#ifndef _HP_H_
#define _HP_H_

/* shortcuts */
#define atomic_load(src) __atomic_load_n(src, __ATOMIC_SEQ_CST)
#define atomic_store(dst, val) __atomic_store(dst, val, __ATOMIC_SEQ_CST)
#define atomic_exchange(ptr, val) \
    __atomic_exchange_n(ptr, val, __ATOMIC_SEQ_CST)
#define atomic_cas(dst, expected, desired)                                 \
    __atomic_compare_exchange(dst, expected, desired, 0, __ATOMIC_SEQ_CST, \
                              __ATOMIC_SEQ_CST)

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define LIST_ITER(head, node) \
    for (node = atomic_load(head); node; node = atomic_load(&node->next))

typedef struct __hp {
    uintptr_t ptr;
    struct __hp *next;
} hp_t;

hp_t *list_insert_or_append(hp_t **head, uintptr_t ptr);
bool list_remove(hp_t **head, uintptr_t ptr);
bool list_contains(hp_t **head, uintptr_t ptr);
void list_free(hp_t **head);

#endif
