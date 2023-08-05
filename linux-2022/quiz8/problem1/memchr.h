/**
 * @file    - memchr.h
 * @brief   - Linux kernel 中不同的 memchr 實作
 */

#ifndef _MEMCHR_H_
#define _MEMCHR_H_

#include <stddef.h>
#include <string.h>

/**
 * memchr_origin - Find a character in an area of memory. (Original source code in Linux kernel)
 * @s: The memory area
 * @c: The byte to search for
 * @n: The size of the area.
 *
 * returns the address of the first occurrence of @c, or %NULL
 * if @c is not found
 */
void *memchr_origin(const void *s, int c, size_t n)
{
	const unsigned char *p = (const unsigned char *) s;
	while (n-- != 0) {
        	if ((unsigned char)c == *p++) {
			return (void *)(p - 1);
		}
	}
	return NULL;
}

/**
 * x86_memchr - Find a character in an area of memory. (x86-64 in Linux kernel)
 * @cs: The memory area
 * @c: The byte to search for
 * @count: The size of the area.
 *
 * returns the address of the first occurrence of @c, or %NULL
 * if @c is not found
 */
void *x86_memchr(const void *cs, int c, size_t count)
{
	int d0;
	void *res;
	if (!count)
		return NULL;
	asm volatile("repne\n\t"
		"scasb\n\t"
		"je 1f\n\t"
		"movq $1,%0\n"
		"1:\tdecq %0"
		: "=D" (res), "=&c" (d0)
		: "a" (c), "0" (cs), "1" (count)
		: "memory");
	return res;
}

#endif