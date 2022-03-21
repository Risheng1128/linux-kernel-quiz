/**
 * @file    - problem4.c
 * @brief   - 練習將 foreach 巨集 予以擴充
 *            參考 https://en.wikipedia.org/wiki/Foreach_loop
 */

#include <stdio.h>
#include <assert.h>

#define _foreach_no_nullval(i, p, arr) \
    assert((i) >= sizeof(arr) / sizeof(arr[0]) || (p))

#define foreach_int(i, ...)                                            \
    for (unsigned _foreach_i = (((i) = ((int[]){__VA_ARGS__})[0]), 0); \
         _foreach_i < sizeof((int[]){__VA_ARGS__}) / sizeof(int);      \
         (i) = ((int[]){__VA_ARGS__, 0})[_foreach_i++])

#define foreach_ptr(i, ...)                                                 \
    for (unsigned _foreach_i =                                              \
             (((i) = (void *) ((typeof(i)[]){__VA_ARGS__})[0]), 0); \
         (i); (i) = (void *) ((typeof(i)[]){__VA_ARGS__,            \
                                                    NULL})[++_foreach_i],   \
                  _foreach_no_nullval(_foreach_i, i,                        \
                                      ((const void *[]){__VA_ARGS__})))

int main(void)
{
    int i;
    foreach_int(i, 0, -1, 100, 0, -99) {
        printf("i is %i\n", i);
    }
    const char *p;
    foreach_ptr(p, "Hello", "world") {
        printf("p is %s\n", p);
    }

    return 0;
}