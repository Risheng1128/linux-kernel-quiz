/**
 * @file    problem3.c
 * @brief   實作輾轉相除法
 */

#include <stdio.h>
#include <stdint.h>

/** 
 * @fn     - gcd64
 * @brief  - 找到兩數的最大公因數
 */
uint64_t gcd64(uint64_t u, uint64_t v)
{
    if (!u || !v) return u | v;
    int shift;
    for (shift = 0; !((u | v) & 1); shift++) {
        u /= 2, v /= 2;
    }
    while (!(u & 1))
        u /= 2;
    do {
        while (!(v & 1))
            v /= 2;
        if (u < v) {
            v -= u;
        } else {
            uint64_t t = u - v;
            u = v;
            v = t;
        }
    } while (v);
    return u << shift;
}


int main(void)
{
    uint64_t a = 89, b = 75;
    printf("test1 = %lu\n", gcd64(a, b));

    return 0;
}