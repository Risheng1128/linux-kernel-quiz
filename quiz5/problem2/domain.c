
#include "domain.h"
#include <stdio.h>

/* Create a new domain on the heap */
domain_t *domain_new(void (*deallocator)(void *))
{
    domain_t *dom = calloc(1, sizeof(domain_t));
    if (!dom)
        return NULL;

    dom->deallocator = deallocator;
    return dom;
}

/* Free a previously allocated domain */
void domain_free(domain_t *dom)
{
    if (!dom)
        return;

    if (dom->pointers)
        list_free(&dom->pointers); // 釋放 hazard pointer

    if (dom->retired)
        list_free(&dom->retired); // 釋放 retired list

    free(dom);
}

/*
 * Load a safe pointer to a shared object. This pointer must be passed to
 * `drop` once it is no longer needed. Returns 0 (NULL) on error.
 */
uintptr_t load(domain_t *dom, const uintptr_t *prot_ptr)
{
    const uintptr_t nullptr = 0;

    while (1) {
        uintptr_t val = atomic_load(prot_ptr);
        // 判斷 val 所擁有的資料是否已經在 hazard pointer 裡
        hp_t *node = list_insert_or_append(&dom->pointers, val);
        if (!node)
            return 0;

        /* Hazard pointer inserted successfully */
        if (atomic_load(prot_ptr) == val)
            return val;

        /*
         * This pointer is being retired by another thread - remove this hazard
         * pointer and try again. We first try to remove the hazard pointer we
         * just used. If someone else used it to drop the same pointer, we walk
         * the list.
         */
        uintptr_t tmp = val;
        if (!atomic_cas(&node->ptr, &tmp, &nullptr))
            list_remove(&dom->pointers, val);
    }
}

/*
 * Drop a safe pointer to a shared object. This pointer (`safe_val`) must have
 * come from `load`
 */
void drop(domain_t *dom, uintptr_t safe_val)
{
    if (!list_remove(&dom->pointers, safe_val))
        __builtin_unreachable();
}

static void cleanup_ptr(domain_t *dom, uintptr_t ptr, int flags)
{
    if (!list_contains(&dom->pointers, ptr)) { /* deallocate straight away */
        dom->deallocator((void *) ptr);
    } else if (flags & DEFER_DEALLOC) { /* Defer deallocation for later */
        list_insert_or_append(&dom->retired, ptr);
    } else { /* Spin until all readers are done, then deallocate */
        while (list_contains(&dom->pointers, ptr))
            ;
        dom->deallocator((void *) ptr);
    }
}

/* Swaps the contents of a shared pointer with a new pointer. The old value will
 * be deallocated by calling the `deallocator` function for the domain, provided
 * when `domain_new` was called. If `flags` is 0, this function will wait
 * until no more references to the old object are held in order to deallocate
 * it. If flags is `DEFER_DEALLOC`, the old object will only be deallocated
 * if there are already no references to it; otherwise the cleanup will be done
 * the next time `cleanup` is called.
 */
void swap(domain_t *dom, uintptr_t *prot_ptr, uintptr_t new_val, int flags)
{
    const uintptr_t old_obj = atomic_exchange(prot_ptr, new_val);
    cleanup_ptr(dom, old_obj, flags);
}

/* Forces the cleanup of old objects that have not been deallocated yet. Just
 * like `swap`, if `flags` is 0, this function will wait until there are no
 * more references to each object. If `flags` is `DEFER_DEALLOC`, only
 * objects that already have no living references will be deallocated.
 */
void cleanup(domain_t *dom, int flags)
{
    hp_t *node;

    LIST_ITER (&dom->retired, node) {
        uintptr_t ptr = node->ptr;
        if (!ptr)
            continue;

        if (!list_contains(&dom->pointers, ptr)) {
            /* We can deallocate straight away */
            if (list_remove(&dom->retired, ptr))
                dom->deallocator((void *) ptr);
        } else if (!(flags & DEFER_DEALLOC)) {
            /* Spin until all readers are done, then deallocate */
            while (list_contains(&dom->pointers, ptr))
                ;
            if (list_remove(&dom->retired, ptr))
                dom->deallocator((void *) ptr);
        }
    }
}