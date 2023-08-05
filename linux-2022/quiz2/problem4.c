/**
 * @file    problem4.c
 * @brief   bitmap 應用
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

size_t improved(uint64_t *bitmap, size_t bitmapsize, uint32_t *out)
{
    size_t pos = 0;
    uint64_t bitset;
    // 計算每個 bitmap 裡的數值
    for (size_t k = 0; k < bitmapsize; ++k) {
        bitset = bitmap[k];
        while (bitset != 0) {
            // 將最低位元為 1 的數值儲存到 t
            uint64_t t = bitset & -bitset;
            // 回傳由低位往高位遇上連續多少個 0 才碰到 1
            int r = __builtin_ctzll(bitset);
            // 將計算出的位置存到 out 裡
            out[pos++] = k * 64 + r;
            // 把最低位元為 1 的 bit 改為 0
            bitset ^= t;
        }
    }
    return pos;
}

int main(void)
{
    uint64_t bitmap[1] = {0xFF};
    uint32_t* out = malloc(8 * sizeof(uint32_t));
    size_t pos = improved(bitmap, 1, out);
    for (size_t i = 0; i < pos; i++) {
        printf("%d\n", *(out + i));
    }
    free(out);
    return 0;
}