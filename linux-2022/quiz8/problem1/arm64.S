/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2021 Arm Ltd.
 */

#include <linux/linkage.h>
#include <asm/assembler.h>

/*
 * Find a character in an area of memory.
 *
 * Parameters:
 *	x0 - buf
 *	x1 - c
 *	x2 - n
 * Returns:
 *	x0 - address of first occurrence of 'c' or 0
 */

#define L(label) .L ## label

#define REP8_01 0x0101010101010101
#define REP8_7f 0x7f7f7f7f7f7f7f7f

#define srcin		x0
#define chrin		w1
#define cntin		x2

#define result		x0

#define wordcnt		x3
#define rep01		x4
#define repchr		x5
#define cur_word	x6
#define cur_byte	w6
#define tmp		x7
#define tmp2		x8

	.p2align 4
	nop
SYM_FUNC_START(__pi_memchr)
	and	chrin, chrin, #0xff		 // chrin = chrin & 0xff
	lsr	wordcnt, cntin, #3		 // wordcnt = cntin >> 3
	cbz	wordcnt, L(byte_loop)	 // 如果 wordcnt 為 0 ，表示字串長度小於 8 ，跳進 byte_loop
	mov	rep01, #REP8_01			 // rep01 = 0x0101010101010101
	mul	repchr, x1, rep01		 // repchr = x1 * rep01 (建立 mask)
	and	cntin, cntin, #7		 // cntin = cntin & 7
L(word_loop):
	ldr	cur_word, [srcin], #8	 // LDR (post-indexing) cur_word = *srcin; srcin += 8;
	sub	wordcnt, wordcnt, #1	 // wordcnt = wordcnt - 1
	eor	cur_word, cur_word, repchr // cur_word = cur_word ^ repchr (用來尋找目標字元，發現的話該位元組位置數值 0x00)
	sub	tmp, cur_word, rep01 	 // tmp = cur_word - rep01
	orr	tmp2, cur_word, #REP8_7f // tmp2 = cur_word | 0x7f7f7f7f7f7f7f7f
	bics	tmp, tmp, tmp2		 // (Bit clear) tmp = tmp & ~tmp2
	b.ne	L(found_word)		 // 判斷是否找到目標字元，有就進到 found_word
	cbnz	wordcnt, L(word_loop) // 如果 wordcnt 還沒有為 0 ，就跳進 word_loop
L(byte_loop):
	cbz	cntin, L(not_found) 	  // 如果 cntin 為 0 ，表示沒有字元，跳進 not_found
	ldrb	cur_byte, [srcin], #1 // b: unsigned byte | cur_byte = *srcin; srcin++;
	sub	cntin, cntin, #1 // cntin--
	cmp	cur_byte, chrin  // 判斷字元是否相同
	b.ne	L(byte_loop) // 如果不同就回到 byte_loop
	sub	srcin, srcin, #1 // srcin--; 因為 ldrb 已經往後移動 srcin 指到的位址，因此需要往前移動一個 byte
	ret
L(found_word):
CPU_LE(	rev	tmp, tmp) // 判斷是否要改變 byte order
	clz	tmp, tmp	  // count leading zeros
	sub	tmp, tmp, #64 // tmp = tmp - 64
	add	result, srcin, tmp, asr #3 # result = srcin + tmp >> 3
	ret
L(not_found):
	mov	result, #0 // 回傳 0
	ret
SYM_FUNC_END(__pi_memchr)
SYM_FUNC_ALIAS_WEAK(memchr, __pi_memchr)
EXPORT_SYMBOL_NOKASAN(memchr)