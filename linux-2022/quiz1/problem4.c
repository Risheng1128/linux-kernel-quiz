/**
 * @file    problem4.c
 * @brief   使用 linux kernel 的 linked list 實作 Longest Consecutive Sequence
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/* 儲存資料的結構 */
struct seq_node {
    int num;
    struct list_head link;
};

/**
 * @fn     - find
 * @brief  - 尋找資料 num 有沒有存在資料節點 seq_node 上
 *
 * @attention 函式邏輯
 * 1. 利用 hash function 算出 hash table (hash) 的位置，這邊的 hash function 為
 * num < 0 ? -num % size : num % size
 * 2. 走訪整個 heads[hash] 接著的 linked list ，尋找資料 num 有沒有存在
 * 3. 如果存在則回傳該節點，沒有則回傳 NULL
 */
static struct seq_node *find(int num, int size, struct list_head *heads)
{
    struct seq_node *node;
    // 利用 hash function 算出 hash table (hash) 的位置
    int hash = num < 0 ? -num % size : num % size;
    // 走訪整個 heads[hash] 接著的 linked list ，尋找資料 num 有沒有存在
    list_for_each_entry (node, &heads[hash], link) {
        if (node->num == num)
            // 如果存在則回傳該節點
            return node;
    }
    // 沒有則回傳 NULL
    return NULL;
}

/**
 * @fn     - longestConsecutive
 * @brief  - 找到最長的連續序列，回傳其長度
 *
 * @attention 函式邏輯
 * 1. 建立整個 hash table ，並初始化
 * 2. 對每一筆資料 num[i] 進行搜尋，如果不存在就建立新的 seq_node 並加到對應的
 * hash table 上
 * 3. 再次收尋每個資料，找到資料節點後，把該節點
 * remove，並且分別往比較小和比較大的值開始尋找，直到兩邊都沒有資料
 * 4. 找完之後和過去找到的長度比較，若比較長則更新，反之則不變
 */
int longestConsecutive(int *nums, int n_size)
{
    int hash, length = 0;
    struct seq_node *node;
    // 建立整個 hash table
    struct list_head *heads = malloc(n_size * sizeof(*heads));

    for (int i = 0; i < n_size; i++)
        INIT_LIST_HEAD(&heads[i]);

    for (int i = 0; i < n_size; i++) {
        // 對每一筆資料 num[i] 進行搜尋
        if (!find(nums[i], n_size, heads)) {
            // 加到對應的 hash table 上
            hash = nums[i] < 0 ? -nums[i] % n_size : nums[i] % n_size;
            node = malloc(sizeof(*node));
            node->num = nums[i];
            list_add(&node->link, &heads[hash]);
        }
    }

    // 再次收尋每個資料
    for (int i = 0; i < n_size; i++) {
        int len = 0;
        int num;
        node = find(nums[i], n_size, heads);
        if (node) {
            len++;
            // 複製資料
            num = node->num;
            // 找到資料節點後，把該節點 remove
            list_del(&node->link);
            free(node);
            // 分別往比較小和比較大的值開始尋找
            int left = num, right = num;
            // 往比較小的值開始找
            while ((node = find(--left, n_size, heads))) {
                len++;
                // 如果存在就 remove
                list_del(&node->link);
                free(node);
            }
            // 往比較大的值開始找
            while ((node = find(++right, n_size, heads))) {
                len++;
                // 如果存在就 remove
                list_del(&node->link);
                free(node);
            }
            // 找完之後和過去找到的長度比較，若比較長則更新，反之則不變
            length = len > length ? len : length;
        }
    }
    free(heads);
    return length;
}

/**
 * @fn     - main
 * @brief  - 測試 Longest Consecutive Sequence 是否正確
 *
 * @attention
 * 範例參考 128. Longest Consecutive Sequence
 * (https://leetcode.com/problems/longest-consecutive-sequence/)
 *
 */
int main(void)
{
    int nums1[6] = {100, 4, 200, 1, 3, 2};
    printf("test1 = %d\n", longestConsecutive(nums1, 6));
    int nums2[10] = {0, 3, 7, 2, 5, 8, 4, 6, 0, 1};
    printf("test2 = %d\n", longestConsecutive(nums2, 10));
    return 0;
}