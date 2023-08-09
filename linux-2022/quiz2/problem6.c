/**
 * @file    problem6.c
 * @brief   類似 __alignof__ (https://gcc.gnu.org/onlinedocs/gcc/Alignment.html)
 * 的實作
 */

#include <stdio.h>

/*
 * ALIGNOF - get the alignment of a type
 * @t: the type to test
 *
 * This returns a safe alignment for the given type.
 */
#define ALIGNOF(t)          \
    ((char *) (&((struct {  \
                    char c; \
                    t _h;   \
                } *) 0)     \
                    ->_h) - \
     (char *) 0)

int main(void)
{
    printf("%ld\n", ALIGNOF(char));
    printf("%ld\n", ALIGNOF(int));
    printf("%ld\n", ALIGNOF(float));
    printf("%ld\n", ALIGNOF(double));
    return 0;
}