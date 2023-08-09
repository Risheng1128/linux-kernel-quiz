/**
 * @file    problem3.c
 * @brief   使用 linux kernel 的 linked list 實作 LRU Cache
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/* Cache 結構 */
typedef struct {
    int capacity, count;
    struct list_head dhead, hheads[];
} LRUCache;

/* 存放資料的結構 */
typedef struct {
    int key, value;
    struct list_head hlink, dlink;
} LRUNode;

/**
 * @fn     - lRUCacheCreate
 * @brief  - 建立 Cache 資料結構並初始化
 *
 * @attention 函式邏輯
 * 1. 建立 Cache 的結構，大小為 sizeof(*obj) + capacity * sizeof(struct
 * list_head)
 * 2. 初始化所有變數 (count, capacity)
 * 3. 初始化 dhead
 * 4. 對每個 hhead[i] 初始化
 *
 */
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

/**
 * @fn     - lRUCacheFree
 * @brief  - 釋放所有記憶體空間
 *
 * @attention 函式邏輯
 * 1. 使用 list_for_each_entry_safe 走訪整個 dhead
 *    - 會有兩個指標 lru 及 n 分別指著前後兩個節點 (lru 前，n 後)
 *    - 釋放 lru 指到的空間後，lru 移到 n 的位置，n 再往下移動，並重複此過程
 * 2. 釋放 cache 結構
 */
void lRUCacheFree(LRUCache *obj)
{
    LRUNode *lru, *n;
    // 使用 list_for_each_entry_safe 走訪整個 dhead
    list_for_each_entry_safe (lru, n, &obj->dhead, dlink) {
        list_del(&lru->dlink);
        // 釋放 lru 指到的空間
        free(lru);
    }
    // 釋放 cache 結構
    free(obj);
}

/**
 * @fn     - lRUCacheGet
 * @brief  - 利用 key 回傳 cache 的資料，無資料則回傳 -1
 *
 * @attention 函式邏輯
 * 1. 利用 hash function 算出 hash table (hash) 的位置，這邊的 hash function 為
 * key % obj->capacity
 * 2. 走訪 hheads[hash] 成員 hlink 的整個 linked list ，尋找 key 是否已經存在
 * 3. 如果找到了 key ，將該資料的 dlink 移到 dhead 的第一個位置，並回傳 value
 * 4. 如果沒有 key 就回傳 -1
 */
int lRUCacheGet(LRUCache *obj, int key)
{
    LRUNode *lru;
    // 算出 hash map 的位置
    int hash = key % obj->capacity;
    // 走訪 hheads[hash] 的 hlink 尋找 key 是否存在
    list_for_each_entry (lru, &obj->hheads[hash], hlink) {
        if (lru->key == key) {
            // 該資料的 dlink 移到 dhead 的第一個位置
            list_move(&lru->dlink, &obj->dhead);
            return lru->value;
        }
    }
    // 沒有 key 就回傳 -1
    return -1;
}

/**
 * @fn     - lRUCachePut
 * @brief  - 添加資料到 Cache 裡
 *
 * @attention 函式邏輯
 * 1. 利用 hash function 算出 hash table (hash) 的位置，這邊的 hash function 為
 * key % obj->capacity
 * 2. 接著以下有三種可能
 *    (1) 走訪 hheads[hash] 成員 hlink 的整個 linked list ，尋找 key
 * 是否已經存在
 *        - 如果存在就更新 value 並將該資料的 dlink 移到 dhead 的第一個位置
 *    (2) key 不存在就進到 (2) 和 (3) 的情況，如果 capcity 已經滿了
 *        - 把 dhead 的最後一個 LRUNode 取出，這邊使用 list_last_entry
 * 可以直接做到
 *        - 接著更新 key 及 value，並將 dlink 及 hlink 分別放進 dhead 和
 * hheads[hash] 的第一個 (3) 如果 capcity 已經沒有滿，建立新的 LRUNode 並增加
 * count
 *        - 更新 key 及 value，並將 dlink 及 hlink 分別放進 dhead 和
 * hheads[hash] 的第一個
 */
void lRUCachePut(LRUCache *obj, int key, int value)
{
    LRUNode *lru;
    // 利用 hash function 算出 hash table (hash) 的位置
    int hash = key % obj->capacity;
    // 走訪 hheads[hash] 成員 hlink 的整個 linked list ，尋找 key 是否已經存在
    list_for_each_entry (lru, &obj->hheads[hash], hlink) {
        // 如果存在就更新 value 並將該資料的 dlink 移到 dhead 的第一個位置
        if (lru->key == key) {
            list_move(&lru->dlink, &obj->dhead);
            lru->value = value;
            return;
        }
    }

    if (obj->count == obj->capacity) {
        // 把 dhead 的最後一個 LRUNode 取出
        lru = list_last_entry(&obj->dhead, LRUNode, dlink);
        list_del(&lru->dlink);
        list_del(&lru->hlink);
    } else {
        // 如果 capcity 已經沒有滿，建立新的 LRUNode 並增加 count
        lru = malloc(sizeof(LRUNode));
        obj->count++;
    }
    // 更新 key 及 value，並將 dlink 及 hlink 分別放進 dhead 和 hheads[hash]
    // 的第一個
    lru->key = key;
    list_add(&lru->dlink, &obj->dhead);
    list_add(&lru->hlink, &obj->hheads[hash]);
    lru->value = value;
}

/**
 * @fn     - main
 * @brief  - 測試 Cache 的功能是否正確
 *
 * @attention
 * 範例參考 146.LRU Cache (https://leetcode.com/problems/lru-cache/)
 *
 */
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