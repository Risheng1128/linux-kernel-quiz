/**
 * @file    - problem10.c
 * @brief   - 以下巨集可得到二個表示式進行整數除法操作時，最接近的整數值
 */

#include <stdio.h>

#define DIVIDE_ROUND_CLOSEST(x, divisor)                       \
    ({                                                         \
        typeof(x) __x = x;                                     \
        typeof(divisor) __d = divisor;                         \
        (((typeof(x)) -1) > 0 || ((typeof(divisor)) -1) > 0 || \
         (((__x) > 0) == ((__d) > 0)))                         \
            ? (((__x) + ((__d) >> 1)) / (__d))                 \
            : (((__x) - ((__d) >> 1)) / (__d));                \
    })

int main(void)
{
    printf("%d\n", DIVIDE_ROUND_CLOSEST(-10, 3));  // -3
    printf("%d\n", DIVIDE_ROUND_CLOSEST(-11, 3));  // -4
    return 0;
}