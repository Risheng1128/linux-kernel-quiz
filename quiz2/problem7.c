/**
 * @file    problem7.c
 * @brief   FizzBuzz (https://en.wikipedia.org/wiki/Fizz_buzz) 兩種 (naive, bitwise) 不同的實作
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void naive(void)
{
    for (unsigned int i = 1; i < 100; i++) {
        if (i % 3 == 0) printf("Fizz");
        if (i % 5 == 0) printf("Buzz");
        if (i % 15 == 0) printf("FizzBuzz");
        if ((i % 3) && (i % 5)) printf("%u", i);
        printf("\n");
    }
}

/** 
 * @fn     - is_divisible
 * @brief  - 回傳是否可以整除，可以回傳 true, 反則回傳 false
 * 
 * @attention 函式邏輯
 * 1. M3 除以 3 表示以 3 為循環，而 +1 是為了要讓乘上 3 的倍數時會 overflow ，而產生一個很小的數
 * 2. M5 除以 5 表示以 5 為循環，而 +1 是為了要讓乘上 5 的倍數時會 overflow ，而產生一個很小的數
 * 3. 搭配以上兩點，我們可以得知 is_divisible() 的實作手法，利用 overflow 判斷是否可以整除
 *    - 當 M 為 M3 時，如果 n * M 產生 overflow 時，表示 n 可以被 3 整除，此時 n * M 是一個很小的數，因此 n * M <= M - 1 也就成立，回傳 true
 *    - 當 M 為 M5 時，如果 n * M 產生 overflow 時，表示 n 可以被 5 整除，此時 n * M 是一個很小的數，因此 n * M <= M - 1 也就成立，回傳 true
 *    - 其他的情況則回傳 false
 */
static inline bool is_divisible(uint32_t n, uint64_t M)
{
    return n * M <= M - 1;
}

static uint64_t M3 = UINT64_C(0xFFFFFFFFFFFFFFFF) / 3 + 1; // 0x55555555555555556
static uint64_t M5 = UINT64_C(0xFFFFFFFFFFFFFFFF) / 5 + 1; // 0x33333333333333334

void bitwise(void)
{
    for (size_t i = 1; i <= 100; i++) {
        uint8_t div3 = is_divisible(i, M3);
        uint8_t div5 = is_divisible(i, M5);
        unsigned int length = (2 << div3) << div5;
        printf("length = %d\n", length);

        char fmt[9];
        strncpy(fmt, &"FizzBuzz%u"[(8 >> div5) >> (div3 << 2)], length);
        fmt[length] = '\0';

        printf(fmt, i);
        printf("\n");
    }
}

int main()
{
    naive();
    bitwise();
    return 0;
}

