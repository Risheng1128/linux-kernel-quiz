/**
 * @file    - problem5.c
 * @brief   - 分析 READ_ONCE 實作
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define __READ_ONCE_SIZE                                  \
    ({                                                    \
        switch (size) {                                   \
        case 1:                                           \
            *(uint8_t *) res = *(volatile uint8_t *) p;   \
            break;                                        \
        case 2:                                           \
            *(uint16_t *) res = *(volatile uint16_t *) p; \
            break;                                        \
        case 4:                                           \
            *(uint32_t *) res = *(volatile uint32_t *) p; \
            break;                                        \
        case 8:                                           \
            *(uint64_t *) res = *(volatile uint64_t *) p; \
            break;                                        \
        default:                                          \
            memcpy((void *) res, (const void *) p, size); \
        }                                                 \
    })

static inline void __read_once_size(const volatile void *p, void *res, int size)
{
    __READ_ONCE_SIZE;
}

#define READ_ONCE(x)                                \
    ({                                              \
        union {                                     \
            typeof(x) __val;                        \
            DECL0;                                  \
        } __u;                                      \
        __read_once_size(&(x), __u.__c, sizeof(x)); \
        __u.__val;                                  \
    })

int main(void)
{
    return 0;
}