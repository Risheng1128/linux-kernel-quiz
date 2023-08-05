/**
 * @file    - time.c
 * @brief   - 量測不同平台 memchr 的實作所花的時間
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "memchr.h"
#include "opt.h"

#define STRSIZE 1000U

long long elapse(struct timespec *start, struct timespec *end)
{
    return (long long) (end->tv_sec - start->tv_sec) * 1e9 + (long long) (end->tv_nsec - start->tv_nsec);
}

int main(void)
{
    char str[STRSIZE];
    const char target = '4';
    struct timespec start, end;
    long long origin_time, opt_time, x86_time;

    memset(str, '0', STRSIZE - 1);
    str[STRSIZE - 1] = '\0';
    
    for (int i = 0; i < STRSIZE - 1; i++) {
        str[i] = target;
        clock_gettime(CLOCK_MONOTONIC, &start);
        memchr_origin(str, target, STRSIZE - 1);
        clock_gettime(CLOCK_MONOTONIC, &end);
        origin_time = elapse(&start, &end);

        clock_gettime(CLOCK_MONOTONIC, &start);
        memchr_opt(str, target, STRSIZE - 1);
        clock_gettime(CLOCK_MONOTONIC, &end);
        opt_time = elapse(&start, &end);

        clock_gettime(CLOCK_MONOTONIC, &start);
        x86_memchr(str, target, STRSIZE - 1);
        clock_gettime(CLOCK_MONOTONIC, &end);
        x86_time = elapse(&start, &end);
        printf("%lld %lld %lld\n", origin_time, opt_time, x86_time);
        str[i] = '0';
    }
    return 0;
}
