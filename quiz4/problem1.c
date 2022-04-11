/**
 * @file    - problem1.c
 * @brief   - 延伸 quiz3/problem7.c ，已知輸入大於 0 ，實作計算 [log2(x)] 的函式
 */

#include <stdio.h>
#include <stdint.h>

int ceil_log2(uint32_t x)
{
    uint32_t r, shift;
    x--;
    // 判斷 x 是否需要 16 個 bit 去儲存
    r = (x > 0xFFFF) << 4;                                                                                                                                    
    x >>= r;
    // 判斷 x 是否需要 8 個 bit 去儲存
    shift = (x > 0xFF) << 3;
    x >>= shift;
    r |= shift;
    // 判斷 x 是否需要 4 個 bit 去儲存
    shift = (x > 0xF) << 2;
    x >>= shift;
    r |= shift;
    // 判斷 x 是否需要 2 個 bit 去儲存
    shift = (x > 0x3) << 1;
    x >>= shift;
    // x >> 1: 加上原本 x 的 MSB
    return (r | shift | x >> 1) + 1;
}

int main(void)
{
    for (int i = 1; i <= 50; i++) {
        printf("log2(%d) = %d\n",i, ceil_log2(i));
    }    
    return 0;
}
