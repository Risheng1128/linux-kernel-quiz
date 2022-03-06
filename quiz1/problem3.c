#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct {
    int capacity, count;
    struct list_head dhead, hheads[];
} LRUCache;
    
typedef struct {
    int key, value;
    struct list_head hlink, dlink;
} LRUNode;

LRUCache *lRUCacheCreate(int capacity)
{
    LRUCache *obj = malloc(sizeof(*obj) + capacity * sizeof(struct list_head));
    obj->count = 0;
    obj->capacity = capacity;
    INIT_LIST_HEAD(&obj->dhead);
    for (int i = 0; i < capacity; i++)
        INIT_LIST_HEAD(&obj->hheads[i]);
    return obj;
}
    
void lRUCacheFree(LRUCache *obj)
{
    LRUNode *lru, *n;
    list_for_each_entry_safe(lru, n, &obj->dhead, dlink) {
        list_del(&lru->dlink);
        free(lru);
    }
    free(obj);
}

int lRUCacheGet(LRUCache *obj, int key)
{
    LRUNode *lru;
    int hash = key % obj->capacity;
    list_for_each_entry(lru, &obj->hheads[hash], hlink) {
        if (lru->key == key) {
            list_move(&lru->dlink, &obj->dhead);
            return lru->value;
        }
    }
    return -1;
}

void lRUCachePut(LRUCache *obj, int key, int value)
{
    LRUNode *lru;
    int hash = key % obj->capacity;
    list_for_each_entry(lru, &obj->hheads[hash], hlink) {
        if (lru->key == key) {
            list_move(&lru->dlink, &obj->dhead);
            lru->value = value;
            return;
        }
    }

    if (obj->count == obj->capacity) {
        lru = list_last_entry(&obj->dhead, LRUNode, dlink);
        list_del(&lru->dlink);
        list_del(&lru->hlink);
    } else {
        lru = malloc(sizeof(LRUNode));
        obj->count++;
    }
    lru->key = key;
    list_add(&lru->dlink, &obj->dhead);
    list_add(&lru->hlink, &obj->hheads[hash]);
    lru->value = value;
}

int main(void) 
{
    LRUCache *test = lRUCacheCreate(2);
    lRUCachePut(test, 1, 1);
    lRUCachePut(test, 2, 2);
    printf("data1 = %d\n", lRUCacheGet(test, 1));
    lRUCachePut(test, 3, 3);
    printf("data2 = %d\n", lRUCacheGet(test, 2));
    lRUCachePut(test, 4, 4);
    printf("data3 = %d\n", lRUCacheGet(test, 1));
    printf("data4 = %d\n", lRUCacheGet(test, 3));
    printf("data5 = %d\n", lRUCacheGet(test, 4));
    lRUCacheFree(test);
    return 0;
}