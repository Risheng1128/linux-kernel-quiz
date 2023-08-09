/**
 * @file    problem1.c
 * @brief   使用 linux kernel 的 hash table 實作 Two Sum
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct hlist_node {
    struct hlist_node *next, **pprev;
};
struct hlist_head {
    struct hlist_node *first;
};
typedef struct {
    int bits;
    struct hlist_head *ht;
} map_t;

#define MAP_HASH_SIZE(bits) (1 << bits)

map_t *map_init(int bits)
{
    map_t *map = malloc(sizeof(map_t));
    if (!map)
        return NULL;

    map->bits = bits;
    map->ht = malloc(sizeof(struct hlist_head) * MAP_HASH_SIZE(map->bits));
    if (map->ht) {
        for (int i = 0; i < MAP_HASH_SIZE(map->bits); i++)
            (map->ht)[i].first = NULL;
    } else {
        free(map);
        map = NULL;
    }
    return map;
}

struct hash_key {
    int key;
    void *data;
    struct hlist_node node;
};

#define container_of(ptr, type, member)               \
    ({                                                \
        void *__mptr = (void *) (ptr);                \
        ((type *) (__mptr - offsetof(type, member))); \
    })

#define GOLDEN_RATIO_32 0x61C88647
static inline unsigned int hash(unsigned int val, unsigned int bits)
{
    /* High bits are more random, so use them. */
    return (val * GOLDEN_RATIO_32) >> (32 - bits);
}

static struct hash_key *find_key(map_t *map, int key)
{
    struct hlist_head *head = &(map->ht)[hash(key, map->bits)];
    for (struct hlist_node *p = head->first; p; p = p->next) {
        struct hash_key *kn = container_of(p, struct hash_key, node);
        if (kn->key == key)
            return kn;
    }
    return NULL;
}

void *map_get(map_t *map, int key)
{
    struct hash_key *kn = find_key(map, key);
    return kn ? kn->data : NULL;
}

void map_add(map_t *map, int key, void *data)
{
    struct hash_key *kn = find_key(map, key);
    if (kn)
        return;

    kn = malloc(sizeof(struct hash_key));
    kn->key = key, kn->data = data;

    struct hlist_head *h = &map->ht[hash(key, map->bits)];
    struct hlist_node *n = &kn->node, *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}

void map_deinit(map_t *map)
{
    if (!map)
        return;

    for (int i = 0; i < MAP_HASH_SIZE(map->bits); i++) {
        struct hlist_head *head = &map->ht[i];
        for (struct hlist_node *p = head->first; p;) {
            struct hash_key *kn = container_of(p, struct hash_key, node);
            struct hlist_node *n = p;
            p = p->next;

            if (!n->pprev) /* unhashed */
                goto bail;

            struct hlist_node *next = n->next, **pprev = n->pprev;
            *pprev = next;
            if (next)
                next->pprev = pprev;
            n->next = NULL, n->pprev = NULL;

        bail:
            free(kn->data);
            free(kn);
        }
    }
    free(map);
}

int *twoSum(int *nums, int numsSize, int target, int *returnSize)
{
    map_t *map = map_init(10);
    *returnSize = 0;
    int *ret = malloc(sizeof(int) * 2);
    if (!ret)
        goto bail;

    for (int i = 0; i < numsSize; i++) {
        int *p = map_get(map, target - nums[i]);
        if (p) { /* found */
            ret[0] = *p, ret[1] = i;
            *returnSize = 2;
            break;
        }

        p = malloc(sizeof(int));
        *p = i;
        map_add(map, nums[i], p);
    }

bail:
    map_deinit(map);
    return ret;
}

/**
 * @fn     - main
 * @brief  - 測試 Two Sum 是否正確
 *
 * @attention
 * 範例參考 1. Two Sum (https://leetcode.com/problems/two-sum/)
 *
 */
int main(void)
{
    int nums1[4] = {2, 7, 11, 15}, returnSize = 0;
    int *res = twoSum(nums1, 4, 9, &returnSize);
    printf("[%d, %d]\n", res[0], res[1]);

    int nums2[3] = {3, 2, 4};
    res = twoSum(nums2, 3, 6, &returnSize);
    printf("[%d, %d]\n", res[0], res[1]);

    int nums3[2] = {3, 3};
    res = twoSum(nums3, 2, 6, &returnSize);
    printf("[%d, %d]\n", res[0], res[1]);

    return 0;
}