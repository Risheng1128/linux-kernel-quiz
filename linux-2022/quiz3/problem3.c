/**
 * @file    - problem3.c
 * @brief   - 將輸入的 8 位元無號整數逐一位元反轉
 *            參考 https://leetcode.com/problems/reverse-bits/submissions/
 */

#include <stdint.h>
#include <stdio.h>

uint8_t rev8(uint8_t x)
{
    // 將低 4 位元和高 4 位元互換位置
    x = (x >> 4) | (x << 4);
    // 將 8-bit 看成兩個部份 (4, 4), 分別將兩部份的高 2 位元和低 2 位元交換
    x = ((x & 0xCC) >> 2) | ((x & 0x33) << 2);
    // 將兩部份的奇數和偶數位元交換
    x = ((x & 0xAA) >> 1) | ((x & 0x55) << 1);
    return x;
}

int main(void)
{
    uint8_t a = 0x87;
    printf("ans = %x\n", rev8(a));
    return 0;
}