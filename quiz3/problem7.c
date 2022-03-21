/**
 * @file    - problem7.c
 * @brief   - 考慮 ilog32 函式可針對給定的 32 位元無號數，計算扣除開頭的 0，最少需要多少位元才可儲存
 */

#include <stdint.h>

int ilog32(uint32_t v)
{
    int ret = v > 0;
    int m = (v > 0xFFFFU) << 4;
    v >>= m;
    ret |= m;
    m = (v > 0xFFU) << 3;
    v >>= m;
    ret |= m;
    m = (v > 0xFU) << 2;
    v >>= m;
    ret |= m;
    m = EXP10;
    v >>= m;
    ret |= m;
    EXP11;
    return ret;
}