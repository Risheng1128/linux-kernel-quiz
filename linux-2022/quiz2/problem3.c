/**
 * @file    problem3.c
 * @brief   實作輾轉相除法
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @fn     - gcd64
 * @brief  - 找到兩數的最大公因數
 */
uint64_t gcd64(uint64_t u, uint64_t v)
{
    if (!u || !v)
        return u | v;
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

/**
 * @fn     - gcd64_ctz
 * @brief  - 找到兩數的最大公因數，並使用 __builtin_ctz 優化
 */
uint64_t gcd64_ctz(uint64_t u, uint64_t v)
{
    if (!u || !v)
        return u | v;
    // 如果兩數都是偶數的話，則可以先將公因數 2 提取出來
    int shift = __builtin_ctz(u | v);
    u >>= shift;
    v >>= shift;
    // 如果兩數還有任一個是 2 的倍數的話，可以直接把該數的因數 2 除掉
    u >>= __builtin_ctz(u);
    do {
        v >>= __builtin_ctz(v);
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

uint64_t randu64()
{
    uint64_t res = 0;
    for (int i = 0; i < 64; i++) {
        res = (res << 1) | (rand() & 1);
    }
    return res;
}

long long elapse(struct timespec *start, struct timespec *end)
{
    return (long long) (end->tv_sec - start->tv_sec) * 1e9 +
           (long long) (end->tv_nsec - start->tv_nsec);
}

int main(void)
{
    struct timespec start, end;
    long long res1 = 0, res2 = 0;
    srand(time(NULL));
    // 執行 50 次
    for (int i = 0; i < 50; i++) {
        // 量測取隨機數 1000000 次後計算 GCD 所花時間的總和
        for (int j = 0; j < 1000000; j++) {
            uint64_t a = randu64();
            uint64_t b = randu64();
            clock_gettime(CLOCK_MONOTONIC, &start);
            gcd64(a, b);
            clock_gettime(CLOCK_MONOTONIC, &end);
            res1 += elapse(&start, &end);
            clock_gettime(CLOCK_MONOTONIC, &start);
            gcd64_ctz(a, b);
            clock_gettime(CLOCK_MONOTONIC, &end);
            res2 += elapse(&start, &end);
        }
        printf("%lld %lld\n", res1, res2);
        res1 = 0;
        res2 = 0;
    }
    return 0;
}