#ifndef _DOMAIN_H_
#define _DOMAIN_H_

#include "hp.h"

#define DEFER_DEALLOC 1

typedef struct {
    hp_t *pointers; // hazard pointers
    hp_t *retired;  // retire list
    void (*deallocator)(void *);
} domain_t;

domain_t *domain_new(void (*deallocator)(void *));
void domain_free(domain_t *dom);
uintptr_t load(domain_t *dom, const uintptr_t *prot_ptr);
void drop(domain_t *dom, uintptr_t safe_val);
void swap(domain_t *dom, uintptr_t *prot_ptr, uintptr_t new_val, int flags);
void cleanup(domain_t *dom, int flags);

#endif