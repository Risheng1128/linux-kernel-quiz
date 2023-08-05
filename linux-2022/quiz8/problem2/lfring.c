#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "arch.h"
#include "common.h"
#include "lfring.h"

#define SUPPORTED_FLAGS \
    (LFRING_FLAG_SP | LFRING_FLAG_MP | LFRING_FLAG_SC | LFRING_FLAG_MC)

#define MIN(a, b)                      \
    ({                                 \
        __typeof__(a) tmp_a = (a);     \
        __typeof__(b) tmp_b = (b);     \
        tmp_a < tmp_b ? tmp_a : tmp_b; \
    })

typedef uintptr_t ringidx_t;
struct element {
    void *ptr;
    uintptr_t idx;
};

struct lfring {
    ringidx_t head;
    ringidx_t tail ALIGNED(CACHE_LINE);
    uint32_t mask;
    uint32_t flags;
    struct element ring[] ALIGNED(CACHE_LINE);
} ALIGNED(CACHE_LINE);

lfring_t *lfring_alloc(uint32_t n_elems, uint32_t flags)
{
    // 指定 ringsz 為 n_elems 往上對齊 2 的倍數
    unsigned long ringsz = ROUNDUP_POW2(n_elems);
    if (n_elems == 0 || ringsz == 0 || ringsz > 0x80000000) {
        assert(0 && "invalid number of elements");
        return NULL;
    }
    if ((flags & ~SUPPORTED_FLAGS) != 0) {
        assert(0 && "invalid flags");
        return NULL;
    }
    // queue 總大小
    size_t nbytes = sizeof(lfring_t) + ringsz * sizeof(struct element);
    lfring_t *lfr = osal_alloc(nbytes, CACHE_LINE);
    if (!lfr)
        return NULL;

    lfr->head = 0, lfr->tail = 0;
    lfr->mask = ringsz - 1;
    lfr->flags = flags;
    for (ringidx_t i = 0; i < ringsz; i++) {
        lfr->ring[i].ptr = NULL;
        lfr->ring[i].idx = i - ringsz;
    }
    return lfr;
}

void lfring_free(lfring_t *lfr)
{
    if (!lfr)
        return;

    if (lfr->head != lfr->tail) {
        assert(0 && "ring buffer not empty");
        return;
    }
    osal_free(lfr);
}

/* True if 'a' is before 'b' ('a' < 'b') in serial number arithmetic */
static inline bool before(ringidx_t a, ringidx_t b)
{
    return (intptr_t)(a - b) < 0;
}

static inline ringidx_t cond_update(ringidx_t *loc, ringidx_t neu)
{
    ringidx_t old = __atomic_load_n(loc, __ATOMIC_RELAXED);
    do {
        if (before(neu, old)) /* neu < old */
            return old;
        /* if neu > old, need to update *loc */
    } while (!__atomic_compare_exchange_n(loc, &old, /* Updated on failure */
                                          neu,
                                          /* weak */ true, __ATOMIC_RELEASE,
                                          __ATOMIC_RELAXED));
    return neu;
}

static inline ringidx_t cond_reload(ringidx_t idx, const ringidx_t *loc)
{
    ringidx_t fresh = __atomic_load_n(loc, __ATOMIC_RELAXED);
    if (before(idx, fresh)) { /* fresh is after idx, use this instead */
        idx = fresh;
    } else { /* Continue with next slot */
	/* XXXXX */ idx++;
    }
    return idx;
}

/* Enqueue elements at tail */
uint32_t lfring_enqueue(lfring_t *lfr,
                        void *const *restrict elems,
                        uint32_t n_elems)
{
    intptr_t actual = 0;
    ringidx_t mask = lfr->mask;
    ringidx_t size = mask + 1;
    ringidx_t tail = __atomic_load_n(&lfr->tail, __ATOMIC_RELAXED);

    if (lfr->flags & LFRING_FLAG_SP) { /* single-producer */
        ringidx_t head = __atomic_load_n(&lfr->head, __ATOMIC_ACQUIRE);
        // 實際上剩下多少空間，可容納 n_elems 或是 head + size - tail (容量不足時)
        actual = MIN((intptr_t)(head + size - tail), (intptr_t) n_elems);
        if (actual <= 0)
            return 0;
        // 根據 actual 數值新增資料
        for (uint32_t i = 0; i < (uint32_t) actual; i++) {
            assert(lfr->ring[tail & mask].idx == tail - size);
            lfr->ring[tail & mask].ptr = *elems++;
            lfr->ring[tail & mask].idx = tail;
            tail++;
        }
        // 將 tail 更新
        __atomic_store_n(&lfr->tail, tail, __ATOMIC_RELEASE);
        // 回傳實際儲存多少筆資料
        return (uint32_t) actual;
    }

    /* else: lock-free multi-producer */
restart:
    // 判斷是否還有空間儲存資料
    while ((uint32_t) actual < n_elems &&
           before(tail, __atomic_load_n(&lfr->head, __ATOMIC_ACQUIRE) + size)) {
        union {
            struct element e;
            ptrpair_t pp;
        } old, neu;
        void *elem = elems[actual];
        // 取得要寫入的資料地址
        struct element *slot = &lfr->ring[tail & mask];
        old.e.ptr = __atomic_load_n(&slot->ptr, __ATOMIC_RELAXED);
        old.e.idx = __atomic_load_n(&slot->idx, __ATOMIC_RELAXED);
        do {
            if (UNLIKELY(old.e.idx != tail - size)) {
                if (old.e.idx != tail) {
                    /* We are far behind. Restart with fresh index */
                    tail = cond_reload(tail, &lfr->tail);
                    goto restart;
                }
                /* slot already enqueued */
                tail++; /* Try next slot */
                goto restart;
            }

            /* Found slot that was used one lap back.
             * Try to enqueue next element.
             */
            neu.e.ptr = elem;
            neu.e.idx = tail; /* Set idx on enqueue */
        } while (!lf_compare_exchange((ptrpair_t *) slot, &old.pp, neu.pp));
        /**
         * 重新判斷 slot 和 old.pp 是否相同
         * 如果相同表示沒有其他執行序影響 slot ，因此將 neu.pp 複製到 slot 並離開迴圈
         * 如果不同表示已經有其他執行序修改 slot ，因此將 slot 複製到 old.pp 並重新執行一次
         */

        /* Enqueue succeeded */
        actual++;
        tail++; /* Continue with next slot */
    }
    (void) cond_update(&lfr->tail, tail);
    return (uint32_t) actual;
}

static inline ringidx_t find_tail(lfring_t *lfr, ringidx_t head, ringidx_t tail)
{
    if (lfr->flags & LFRING_FLAG_SP) /* single-producer enqueue */
        return __atomic_load_n(&lfr->tail, __ATOMIC_ACQUIRE);

    /* Multi-producer enqueue.
     * Scan ring for new elements that have been written but not released.
     */
    ringidx_t mask = lfr->mask;
    ringidx_t size = /* XXXXX */ mask + 1;
    while (before(tail, head + size) &&
           __atomic_load_n(/* XXXXX */ &lfr->ring[tail & mask].idx, __ATOMIC_RELAXED) ==
               tail)
        tail++;
    tail = cond_update(&lfr->tail, tail);
    return tail;
}

/* Dequeue elements from head */
uint32_t lfring_dequeue(lfring_t *lfr,
                        void **restrict elems,
                        uint32_t n_elems,
                        uint32_t *index)
{
    ringidx_t mask = lfr->mask;
    intptr_t actual;
    ringidx_t head = __atomic_load_n(&lfr->head, __ATOMIC_RELAXED);
    ringidx_t tail = __atomic_load_n(&lfr->tail, __ATOMIC_ACQUIRE);
    do {
        // 實際上有多少筆資料，可以取得 n_elems 或 tail - head (資料不足)
        actual = MIN((intptr_t)(tail - head), (intptr_t) n_elems);
        if (UNLIKELY(actual <= 0)) {
            /* Ring buffer is empty, scan for new but unreleased elements */
            tail = find_tail(lfr, head, tail);
            actual = MIN((intptr_t)(tail - head), (intptr_t) n_elems);
            if (actual <= 0)
                return 0;
        }
        // 將資料複製到 elems
        for (uint32_t i = 0; i < (uint32_t) actual; i++)
            elems[i] = lfr->ring[(head + i) & mask].ptr;
        // Memory barrier
        smp_fence(LoadStore);                        // Order loads only
        if (UNLIKELY(lfr->flags & LFRING_FLAG_SC)) { /* Single-consumer */
            // 移動 head ，往後移動 actual 個資料
            __atomic_store_n(&lfr->head, head + actual, __ATOMIC_RELAXED);
            break;
        }

        /* else: lock-free multi-consumer */
    } while (!__atomic_compare_exchange_n(
        &lfr->head, &head, /* Updated on failure */
        /* XXXXX */ head + actual,
        /* weak */ false, __ATOMIC_RELAXED, __ATOMIC_RELAXED));
    /**
     * 重新判斷 lfr->head 和 head 是否相同
     * 如果相同表示沒有其他執行序影響 lfr->head ，因此將 head + actual 複製到 lfr->head 並離開迴圈
     * 如果不同表示已經有其他執行序修改 lfr->head ，因此將 lfr->head 複製到 head 並重新執行一次
     */

    *index = (uint32_t) head;
    return (uint32_t) actual;
}