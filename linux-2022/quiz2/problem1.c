/**
 * @file    problem1.c
 * @brief   兩數平均的各種實作
 */

#include <stdint.h>
#include <stdio.h>

uint32_t average(uint32_t a, uint32_t b)
{
    return (a + b) / 2;
}

uint32_t average1(uint32_t low, uint32_t high)
{
    return low + (high - low) / 2;
}

uint32_t average2(uint32_t a, uint32_t b)
{
    return (a >> 1) + (b >> 1) + (a & b & 1U);
}

uint32_t average3(uint32_t a, uint32_t b)
{
    return (a & b) + ((a ^ b) >> 1);
}

int main(void)
{
    uint32_t a = 3, b = 5;
    printf("test1 = %x\n", average(a, b));
    printf("test2 = %x\n", average1(a, b));
    printf("test3 = %x\n", average2(a, b));
    printf("test4 = %x\n", average3(a, b));
    return 0;
}