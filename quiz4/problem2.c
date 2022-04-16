/**
 * @file    - problem2.c
 * @brief   - 延伸 quiz3/problem11.c ，改寫測驗 11 裡的 fls 函式 (find last set)
 */

#define BITS_PER_BYTE 8
#define BITS_PER_LONG (sizeof(unsigned long) * BITS_PER_BYTE)

#include <stdio.h>
#include <stddef.h>

static inline size_t ffs(unsigned long x)
{
    if (x == 0)
        return 0;

    size_t o = 1;
    unsigned long t = ~0UL;
    size_t shift = BITS_PER_LONG;

    shift >>= 1;
    t >>= shift;

    while (shift) {
        if ((EXP2) == 0) {
            x >>= shift;
            EXP3;
        }

        shift >>= 1;
        t >>= shift;
    }

    return o;
}

int main(void)
{
    return 0;
}
