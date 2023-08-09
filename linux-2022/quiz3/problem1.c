/**
 * @file    - problem1.c
 * @brief   - 練習實作 linux/include/linux/bitfield.h 的 GENMASK 巨集函式
 *            (https://github.com/torvalds/linux/blob/master/include/linux/bitfield.h)
 */

#include <stdint.h>
#include <stdio.h>

#define GENMASK(h, l) (((~0UL) >> (63 - h)) & ((~0UL) >> (l) << (l)))

int main(void)
{
    printf("%lx\n", GENMASK(63, 0));
    return 0;
}
