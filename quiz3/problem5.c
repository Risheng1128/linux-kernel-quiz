/**
 * @file    - problem5.c
 * @brief   - 實作兩個有號整數的除法
 *            參考 https://leetcode.com/problems/divide-two-integers/
 */

#include <stdio.h>
#include <limits.h>

/**
 *  @fn     divide()
 *  @brief  回傳兩個有號整數的除法
 */
int divide(int dividend, int divisor)
{
    int signal = 1;
    unsigned int dvd = dividend;
    /** 
     * 判斷 dividend 是否小於 0
     * 如果是，將 signal 變號 (可用 bitwise 優化)
     * 如果是，利用 2's complement 將 dvd 從負數編碼變成正數 
     */
    if (dividend < 0) {
        signal *= -1;
        dvd = ~dvd + 1;
    }

    unsigned int dvs = divisor;
    /** 
     * 判斷 divisor 是否小於 0
     * 如果是，將 signal 變號 (可用 bitwise 優化)
     * 如果是，利用 2's complement 將 dvs 從負數編碼變成正數 
     */
    if (divisor < 0) {
        signal *= -1;
        dvs = ~dvs + 1;
    }

    int shift = 0;
    // 計算 dvs << shift 最接近 dvd 但大於等於 dvd 的 shift 數值
    while (dvd > (dvs << shift))
        shift++;

    unsigned int res = 0;
    while (dvd >= dvs) {
        // 計算 dvs << shift 最接近 dvd 但小於等於 dvd 的 shift 數值
        while (dvd < (dvs << shift))
            shift--;
        // res += 2^shift (加上每一次迭代的商)
        res |= (unsigned int) 1 << shift;
        // 將 dvd 減去商乘上除數
        dvd -= dvs << shift;
    }

    // 如果 signal 為 1 表示結果為正數，但商也大於 INT_MAX 時進入，直接回傳 INT_MAX
    if (signal == 1 && res >= INT_MAX)
        return INT_MAX;
    // 回傳整數的商 (要乘上正負號)
    return res * signal;
}

int main(void)
{
    printf("test1 = %d\n", divide(60, 3));
    printf("test2 = %d\n", divide(27, -3));
    return 0;
}