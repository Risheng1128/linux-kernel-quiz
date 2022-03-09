/**
 * @file    problem5.c
 * @brief   bitmap 應用
 */

#include <stdbool.h>                       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/* Rem 結構 */
struct rem_node {
    int key;   // 紀錄餘數
    int index; // 紀錄小數點位數
    struct list_head link;
};

/** 
 * @fn     - find
 * @brief  - 尋找資料 key 有沒有存在資料節點 rem_node 上
 * 
 * @attention 函式邏輯
 * 1. 利用 hash function 算出 hash table (hash) 的位置，這邊的 hash function 為 key % size
 * 2. 走訪整個 heads[hash] 接著的 linked list ，尋找資料 num 有沒有存在
 * 3. 如果存在則回傳該節點成員 index ，沒有則回傳 -1
 */
static int find(struct list_head *heads, int size, int key)
{
    struct rem_node *node;
    // 利用 hash function 算出 hash table (hash) 的位置
    int hash = key % size;
    // 走訪整個 heads[hash] 接著的 linked list，尋找資料 num 有沒有存在
    list_for_each_entry (node, &heads[hash], link) {
        if (key == node->key)
            // 如果存在則回傳該節點成員 index
            return node->index;
    }
    // 沒有則回傳 -1
    return -1;
}

/** 
 * @fn     - free_ht
 * @brief  - 釋放 hash table
 */
static void free_ht(struct list_head *heads, int size)
{
    struct rem_node *node, *next;
    for (int i = 0; i < size; i++) {
        list_for_each_entry_safe (node, next, &heads[i], link) {
            list_del(&node->link);
            free(node);
        }
    }
    free(heads);
}

/** 
 * @fn     - fractionToDecimal
 * @brief  - 回傳兩整數除完的數值 (利用字串表示小數)
 * 
 * @attention 函式邏輯
 * 1. 如果 denominator 為 0，回傳 '/0'
 * 2. 如果 numerator 為 0，回傳 "0"
 * 3. 如果 denominator 或 numerator 有負數，則先變回正數
 * 4. 先做第一次的除法，處理整數位，如果沒有餘數則直接回傳
 * 5. 開啟大小為 size 的 hash table
 * 6. 開始進行除法計算
 * 7. 如果整除則會離開迴圈，並把 decimal 的資料複製給 p
 * 8. 如果出現無限迴圈會進入 if(pos >= 0)
 */
char *fractionToDecimal(int numerator, int denominator)
{
    int size = 1024;
    char *result = malloc(size);
    char *p = result;
    
    // 如果 denominator 為 0，回傳 '/0'
    if (denominator == 0) {
        result[0] = '\0';
        return result;
    }
    
    // 如果 numerator 為 0，回傳 "0"
    if (numerator == 0) {
        result[0] = '0';
        result[1] = '\0';
        return result;
    }

    /* using long long type make sure there has no integer overflow */
    long long n = numerator;
    long long d = denominator;

    /* deal with negtive cases */
    int mask = n >> 31;
    n = (n + mask) ^ mask;
    mask = d >> 31;
    d = (d + mask) ^ mask;

    bool sign = (numerator < 0) ^ (denominator < 0);
    if (sign)
        *p++ = '-';
    
    // 先做第一次的除法，處理整數位
    long long remainder = n % d;
    long long division = n / d;

    sprintf(p, "%ld", division > 0 ? (long) division : (long) -division);
    if (remainder == 0)
        return result;

    p = result + strlen(result);
    *p++ = '.';

    /* Using a map to record all of reminders and their position.
     * if the reminder appeared before, which means the repeated loop begin,
     */
    char *decimal = malloc(size);
    memset(decimal, 0, size);
    char *q = decimal;

    size = 1333;
    // 開啟大小為 size 的 hash table
    struct list_head *heads = malloc(size * sizeof(*heads));
    for (int i = 0; i < size; i++)
        INIT_LIST_HEAD(&heads[i]);

    // 開始進行除法計算
    for (int i = 0; remainder; i++) {
        int pos = find(heads, size, remainder);
        // 出現無窮小數的情況
        if (pos >= 0) {
            q = decimal;
            // 先加上不是循環小數的部份
            while (pos-- > 0)
                *p++ = *decimal++;
            *p++ = '(';
            // 加上循環小數的部份 用 "()" 括住
            while (*decimal != '\0')
                *p++ = *decimal++;
            *p++ = ')';
            *p = '\0';
            free_ht(heads, size);
            free(q);
            return result;
        }
        struct rem_node *node = malloc(sizeof(*node));
        node->key = remainder; // 紀錄餘數
        node->index = i; // 紀錄位數

        list_add(&node->link, &heads[remainder % size]);
        *q++ = (remainder * 10) / d + '0'; // 新的小數點位數
        remainder = (remainder * 10) % d;  // 新的餘數
    }
    // 已經整除
    strcpy(p, decimal);
    free_ht(heads, size);
    free(decimal);
    return result;
}

void Comput_and_Display(int numerator, int denominator)
{
    char* res = fractionToDecimal(numerator, denominator);
    printf("res = %s\n", res);
    free(res);
}

/** 
 * @fn     - main
 * @brief  - 測試 fractionToDecimal 的正確性
 */
int main(void)
{
    Comput_and_Display(1, 2);
    Comput_and_Display(4, 333);
    return 0;
}
