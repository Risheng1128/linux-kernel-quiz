/**
 * @file    - problem9.c
 * @brief   - 實作記憶體地址對齊 (alignment) 的巨集，作用為向上對齊 (roundup)
 */

/* maximum alignment needed for any type on this platform, rounded up to a
   power of two */
#define MAX_ALIGNMENT 16

/* Given a size, round up to the next multiple of sizeof(void *) */
#define ROUND_UP_TO_ALIGNMENT_SIZE(x) \
    (((x) + MAX_ALIGNMENT - MMM) & ~(NNN))