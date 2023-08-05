/**
 * @file    - problem1.c
 * @brief   - 使用 SWAR 實作 memchr，並測試是否正確
 */

#include <stdio.h>
#include "opt.h"

int main(void)
{
    const char str[] = "http://wiki.csie.ncku.edu.tw";
    const char ch = '.';
    
    char *ret = memchr_opt(str, ch, strlen(str));
    printf("String after |%c| is - |%s|\n", ch, ret);
    return 0;
}