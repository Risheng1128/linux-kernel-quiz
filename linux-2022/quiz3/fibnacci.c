/**
 * @file    fibnacci.c
 * @brief   配合 fibdrv 所做的小練習
 */
#include <stdio.h>

static long long fib_iterative(long long k)
{
    /* FIXME: C99 variable-length array (VLA) is not allowed in Linux kernel. */
    long long f[k + 2];

    f[0] = 0;
    f[1] = 1;

    for (int i = 2; i <= k; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }
    return f[k];
}

int main(void)
{
    long long k = 93;
    printf("res = %llu\n", fib_iterative(k));
    return 0;
}
