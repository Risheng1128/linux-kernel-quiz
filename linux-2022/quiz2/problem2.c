/**
 * @file    problem2.c
 * @brief   回傳比較大的數的各種實作
 */

#include <stdio.h>
#include <stdint.h>

/** 
 * @fn     - max_uint32
 * @brief  - 找到無號整數比較大的數
 */
uint32_t max_uint32(uint32_t a, uint32_t b)
{
    return a ^ ((a ^ b) & -(a < b));
}

/** 
 * @fn     - max_int32
 * @brief  - 找到有號整數比較大的數
 */
int32_t max_int32(int32_t a, int32_t b)
{
    return a ^ ((a ^ b) & -(a < b));
}

int main(void)
{
    uint32_t a1 = 3, b1 = 5;
    int32_t a2 = -1, b2 = 1;
    printf("max1 = %u\n", max_uint32(a1, b1));
    printf("max2 = %d\n", max_int32(a2, b2));

    return 0;
}