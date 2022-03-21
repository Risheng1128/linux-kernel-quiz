/**
 * @file    - problem3.c
 * @brief   - 將輸入的 8 位元無號整數逐一位元反轉
 *            參考 https://leetcode.com/problems/reverse-bits/submissions/
 */

#include <stdint.h>
#include <stdio.h>

uint8_t rev8(uint8_t x)
{
    x = (x >> 4) | (x << 4);               
    x = ((x & 0xCC) >> 2) | ((x << 2) & 0xCC);
    x = ((x & 0xAA) >> 1) | ((x << 1) & 0xAA);
    return x;
}

int main(void)
{
    uint8_t a = 0x55;
    printf("ans = %x\n", rev8(a));
    return 0;
}