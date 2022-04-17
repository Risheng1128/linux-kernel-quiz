/**
 * @file    - opt.h
 * @brief   - 使用 SWAR 實作 memchr
 */

#ifndef _OPT_H_
#define _OPT_H_

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

/* Nonzero if either X or Y is not aligned on a "long" boundary */
#define UNALIGNED(X) ((long) X & (sizeof(long) - 1))

/* How many bytes are loaded each iteration of the word copy loop */
#define LBLOCKSIZE (sizeof(long))

/* Threshhold for punting to the bytewise iterator */
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)

#if LONG_MAX == 2147483647L
#define DETECT_NULL(X) (((X) -0x01010101) & ~(X) & 0x80808080)
#else
#if LONG_MAX == 9223372036854775807L
/* Nonzero if X (a long int) contains a NULL byte. */
#define DETECT_NULL(X) (((X) -0x0101010101010101) & ~(X) & 0x8080808080808080)
#else
#error long int is not a 32bit or 64bit type.
#endif
#endif

/* @return nonzero if (long)X contains the byte used to fill MASK. */
#define DETECT_CHAR(X, MASK) (DETECT_NULL(X ^ MASK))

void *memchr_opt(const void *src_void, int c, size_t length)
{
    const unsigned char *src = (const unsigned char *) src_void;
    unsigned char d = c;

    // 如果 src 的地址沒有對齊 8 byte ，就一個一個字元找
    while (UNALIGNED(src)) {
        if (!length--)
            return NULL;
        if (*src == d)
            return (void *) src;
        src++;
    }

    // 表示 length > LBLOCKSIZE
    if (!TOO_SMALL(length)) {
        /* If we get this far, we know that length is large and
         * src is word-aligned.
         */

        /* The fast code reads the source one word at a time and only performs
         * the bytewise search on word-sized segments if they contain the search
         * character, which is detected by XORing the word-sized segment with a
         * word-sized block of the search character and then detecting for the
         * presence of NULL in the result.
         */
        unsigned long *asrc = (unsigned long *) src;
        // 建立 Mask ，讓 mask 的每個 byte 都是目標字元
        unsigned long mask = d << 8 | d;
        mask = mask << 16 | mask;
        for (unsigned int i = 32; i < LBLOCKSIZE * 8; i <<= 1)
            mask = (mask << i) | mask;

        while (length >= LBLOCKSIZE) {
            /* XXXXX: Your implementation should appear here */
            unsigned long tmp;
            // 包含 NULL ，表示已經找到目標字元
            if ((tmp = DETECT_CHAR(*asrc, mask))) {
                // 計算 asrc 要移動多少 byte
                unsigned long shift = __builtin_ctzl(tmp) >> 3;
                // 回傳 asrc 移動後指到的地址
                return (char *) asrc + shift;
            }
            // asrc 指到下 8 個 byte 的位置
            asrc++;
            // length 剪掉 LBLOCKSIZE 的長度
            length -= LBLOCKSIZE;
        }

        /* If there are fewer than LBLOCKSIZE characters left, then we resort to
         * the bytewise loop.
         */
        src = (unsigned char *) asrc;
    }

    // 最後當長度小於 LBLOCKSIZE 時如果還沒有找到目標字元，剩下的部份一個一個找
    while (length--) {
        if (*src == d)
            return (void *) src;
        src++;
    }
    return NULL;
}

#endif