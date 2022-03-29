/**
 * @file    - problem5.c
 * @brief   - 實作兩個有號整數的除法
 *            參考 https://leetcode.com/problems/divide-two-integers/
 */

#include <stdio.h>
#include <limits.h>

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
    while (dvd > (dvs << shift))
        shift++;

    unsigned int res = 0;
    while (dvd >= dvs) {
        while (dvd < (dvs << shift))
            shift--;
        res |= (unsigned int) 1 << shift;
        dvd -= dvs << shift;
    }

    if (signal == 1 && res >= INT_MAX)
        return INT_MAX;
    return res * signal;
}

int main(void)
{
    printf("test1 = %d\n", divide(60, 3));
    printf("test2 = %d\n", divide(27, -3));
    return 0;
}