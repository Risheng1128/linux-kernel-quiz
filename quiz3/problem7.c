/**
 * @file    - problem7.c
 * @brief   - 考慮 ilog32 函式可針對給定的 32 位元無號數，計算扣除開頭的 0，最少需要多少位元才可儲存
 */

#include <stdint.h>
#include <stdio.h>

int ilog32(uint32_t v)
{
    int ret = v >= 0;
    /**
     * 如果 v > 0xFFFFU ，表示 v 需要至少使用 16 個位元儲存，此時 m = 16
     * 如果 v <= 0xFFFFU，表示 v 不需要用到 16 個位元儲存，此時 m = 0
     */
    int m = (v > 0xFFFFU) << 4;
    /**
     * 如果 v > 0xFFFFU ，將 v 右移 16，接著只考慮高於 17 位元的位置即可 
     * 如果 v <= 0xFFFFU，則 v 將不會改變
     */
    v >>= m;
    // 使用 OR 加上會使用到的位元數
    ret |= m;
    /**
     * 如果 v > 0xFFU ，表示 v 需要至少使用 8 個位元儲存，此時 m = 8
     * 如果 v <= 0xFFU，表示 v 不需要用到 8 個位元儲存，此時 m = 0
     */
    m = (v > 0xFFU) << 3;
    /**
     * 如果 v > 0xFFU ，將 v 右移 8，接著只考慮高於 8 位元的位置即可 
     * 如果 v <= 0xFFU，則 v 將不會改變
     */
    v >>= m;
    // 使用 OR 加上會使用到的位元數
    ret |= m;
    /**
     * 如果 v > 0xFU ，表示 v 需要至少使用 4 個位元儲存，此時 m = 4
     * 如果 v <= 0xFU，表示 v 不需要用到 4 個位元儲存，此時 m = 0
     */
    m = (v > 0xFU) << 2;
    /**
     * 如果 v > 0xFU ，將 v 右移 4，接著只考慮高於 4 位元的位置即可 
     * 如果 v <= 0xFU，則 v 將不會改變
     */
    v >>= m;
    // 使用 OR 加上會使用到的位元數
    ret |= m;
    /**
     * 如果 v > 0x3U ，表示 v 需要至少使用 2 個位元儲存，此時 m = 2
     * 如果 v <= 0x3U，表示 v 不需要用到 2 個位元儲存，此時 m = 0
     */
    m = (v > 0x3U) << 1;
    /**
     * 如果 v > 0x3U ，將 v 右移 2，接著只考慮高於 2 位元的位置即可 
     * 如果 v <= 0x3U，則 v 將不會改變
     */
    v >>= m;
    // 使用 OR 加上會使用到的位元數
    ret |= m;
    // 如果最後 v > 1 ， ret 要在加 1  
    ret += v > 1;
    return ret;
}

int main(void)
{
    uint32_t test = 0x7FFFFF;
    printf("res = %d\n", ilog32(test));
    return 0;
}