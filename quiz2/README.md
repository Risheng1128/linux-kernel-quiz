---
tags: 2022 linux kernel
---

# 2022q1 Homework2 (quiz2)
contributed by < [`Risheng1128`](https://github.com/Risheng1128) >

> [作業說明](https://hackmd.io/@sysprog/BybKYf5xc)
> [測驗題目](https://hackmd.io/@sysprog/linux2022-quiz2)

## 測驗 `1`
:::success
延伸問題:
- [x] 解釋上述程式碼運作的原理
- [ ] 比較上述實作在編譯器最佳化開啟的狀況，對應的組合語言輸出，並嘗試解讀 (可研讀 [CS:APP 第 3 章](https://hackmd.io/@sysprog/CSAPP/https%3A%2F%2Fhackmd.io%2Fs%2FHJDRfVCFG))
- [ ] 研讀 Linux 核心原始程式碼 [include/linux/average.h](https://github.com/torvalds/linux/blob/master/include/linux/average.h)，探討其 [Exponentially weighted moving average (EWMA)](https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average) 實作，以及在 Linux 核心的應用
  移動平均（Moving average)，又稱滾動平均值、滑動平均，在統計學中是種藉由建立整個資料集合中不同子集的一系列平均數，來分析資料點的計算方法。
  移動平均通常與時間序列資料一起使用，以消除短期波動，突出長期趨勢或周期。短期和長期之間的閾值取決於應用，移動平均的參數將相應地設置。例如，它通常用於對財務數據進行技術分析，如股票價格、收益率或交易量。它也用於經濟學中研究國內生產總值、就業或其他宏觀經濟時間序列。
:::

### 程式運作原理
首先，討論以下程式碼
```c
#include <stdint.h>
uint32_t average(uint32_t low, uint32_t high)
{
    return low + (high - low) / 2;
}
```
:::info
思考思路
- 在已知大小的情況下，這樣的寫法可以想像是在 `low` 的大小加上 `high` 到 `low` 之間距離的一半，因此可以得到一半的數字
- 當然，也可以換個思考，改成減少 `high` 的大小，如以下程式碼
  ```c
  uint32_t average(uint32_t low, uint32_t high)
  {
      return high - (high - low) / 2;
  }
  ```
:::

接著討論下一個程式碼
```c
uint32_t average(uint32_t a, uint32_t b)
{
    return (a >> 1) + (b >> 1) + (EXP1);
}
```
:::info
思考思路
- 這邊的話，可以先想成 `a >> 1` 和 `b >> 1` 分別為 `a / 2` 和 `b / 2` ，接著會發現如果只有 `a >> 1 + b >> 1` ，在某些情況下會有問題，例如: `a = 3, b = 5` 出來的結果不是 `4` 而是 `3`
- 稍微研究一下發現當兩個數的 LSB 為 `1` 時，兩數相加的時候 LSB 的位置會進位，但本題的範例是直接先都往右平移 `1` ，因此會把進位忽略掉，從這點可以推斷出 `EXP1` 是和進位有關，加上題目的提示，可以得到以下解答
`EXP1 = a & b & 1` 
:::

接下來是最後一種寫法
```c
uint32_t average(uint32_t a, uint32_t b)
{
    return (EXP2) + ((EXP3) >> 1);
}
```
:::info
思考思路
- 這邊就比較難想，因此先看了題目下方的提示，得知 `EXP2` 和 `EXP3` 都是 `a` 和 `b` 的某種運算 (`OR` `AND` `XOR`)
- 結果後來是用湊答案湊出來的QQ
  `EXP2 = a & b`
  `EXP3 = a ^ b`
- 雖然答案是湊出來的，還是要理解一下為什麼會寫出這樣的怪招，這邊參考 [laneser](https://hackmd.io/@laneser/linux2022-quiz2#2022q1-Homework2-quiz2) 的共筆
  1. 從共筆可以看到其實加法可以寫成 `a + b = (a ^ b) + ((a & b) << 1)` 的形式
  2. 得知了 `a + b` 的寫法後平均就不難了，可以寫成以下的形式
     `(a + b)/2 = ((a ^ b) + ((a & b) << 1)) >> 1`
     &rarr; `(a & b) + ((a ^ b) >> 1)`
:::

### 組合語言輸出
參考 [GCC Command Options](https://web.mit.edu/rhel-doc/3/rhel-gcc-en-3/optimize-options.html) ，裡面說明了 gcc 的各種最佳化及最佳化選項

採用以下的程式碼作為範例，這邊編譯器使用 `-O1` 最佳化選項，再對編譯後的執行檔做反組譯
```c
#include <stdio.h>
#include <stdint.h>

uint32_t average(uint32_t a, uint32_t b)
{
    return (a + b) / 2;
}

uint32_t average1(uint32_t low, uint32_t high)
{
    return low + (high - low) / 2;
}

uint32_t average2(uint32_t a, uint32_t b)
{
    return (a >> 1) + (b >> 1) + (a & b & 1U);
}

uint32_t average3(uint32_t a, uint32_t b)
{
    return (a & b) + ((a ^ b) >> 1);
}

int main(void)
{
   uint32_t a = 3, b = 5;
   printf("test1 = %x\n", average(a, b));
   printf("test2 = %x\n", average1(a, b));
   printf("test3 = %x\n", average2(a, b));
   printf("test4 = %x\n", average3(a, b));
   return 0;
}
```

輸入命令 `objdump -d problem1.out > problem1.d` 以下為輸出的結果
```
0000000000001149 <average>:
    1149:	f3 0f 1e fa          	endbr64 
    114d:	8d 04 37             	lea    (%rdi,%rsi,1),%eax
    1150:	d1 e8                	shr    %eax
    1152:	c3                   	retq   

0000000000001153 <average1>:
    1153:	f3 0f 1e fa          	endbr64 
    1157:	89 f0                	mov    %esi,%eax
    1159:	29 f8                	sub    %edi,%eax
    115b:	d1 e8                	shr    %eax
    115d:	01 f8                	add    %edi,%eax
    115f:	c3                   	retq   

0000000000001160 <average2>:
    1160:	f3 0f 1e fa          	endbr64 
    1164:	89 f8                	mov    %edi,%eax
    1166:	d1 e8                	shr    %eax
    1168:	89 f2                	mov    %esi,%edx
    116a:	d1 ea                	shr    %edx
    116c:	01 d0                	add    %edx,%eax
    116e:	21 f7                	and    %esi,%edi
    1170:	83 e7 01             	and    $0x1,%edi
    1173:	01 f8                	add    %edi,%eax
    1175:	c3                   	retq   

0000000000001176 <average3>:
    1176:	f3 0f 1e fa          	endbr64 
    117a:	89 f8                	mov    %edi,%eax
    117c:	31 f0                	xor    %esi,%eax
    117e:	d1 e8                	shr    %eax
    1180:	21 f7                	and    %esi,%edi
    1182:	01 f8                	add    %edi,%eax
    1184:	c3                   	retq   
```
:::warning
ToDo: 研讀 CS:APP 第三章，解讀上述組合語言，並新增其他優化選項
:::

---
## 測驗 `2`
:::success
延伸問題:
- [x] 解釋上述程式碼運作的原理
- [x] 針對 32 位元無號/有號整數，撰寫同樣 branchless 的實作
- [x] Linux 核心也有若干 branchless / branch-free 的實作，例如 [lib/sort.c](https://github.com/torvalds/linux/blob/master/lib/sort.c):
    ```c
    /*
     * Logically, we're doing "if (i & lsbit) i -= size;", but since the
     * branch is unpredictable, it's done with a bit of clever branch-free
     * code instead.
     */
    __attribute_const__ __always_inline
    static size_t parent(size_t i, unsigned int lsbit, size_t size)
    {
        i -= size;
        i -= size & -(i & lsbit);
        return i / 2;
    }
    ```
    請在 Linux 核心原始程式碼中，找出更多類似的實作手法。請善用 git log 檢索
:::

### 程式運作原理
首先，討論以下程式碼，程式目的為回傳比較大的值
```c
#include <stdint.h>
uint32_t max(uint32_t a, uint32_t b)
{
    return a ^ ((EXP4) & -(EXP5));
}
```

:::info
思考思路
1. 目的是要找到比較大的值，可以合理推論回傳值為 `a` 或 `b` ，首先看到回傳有一個 `a ^ ` 的過程，從老師給的參考連結 [That XOR Trick](https://florian.github.io/xor-trick/) ，發現以下 XOR 的特性
   - `a ^ 0 = a`
   - `a ^ a ^ b = b`
2. 有了以上的特性，可以得知 `EXP4` 和 `-(EXP5)` 經過 `AND` 後的結果
   - 如果 `a` &geq; `b` ,則 `(EXP4) & -(EXP5) = 0`
   - 如果 `a` < `b` ,則 `(EXP4) & -(EXP5) = a ^ b`
3. 從以上的特性加上題目敘述(`EXP4` 為 `a` 和 `b` 進行某種特別 bitwise 操作)，可以先得出 `EXP4` 為 `a ^ b`
4. 繼續思考，將 `EXP4 = a ^ b` 套回第 `2` 點的特性，可以得到更精簡的結果
   - 如果 `a` &geq; `b` ,則 `EXP5 = 0`
   - 如果 `a` < `b` ,則 `EXP5 = 1`
5. 由上述的結論，可以推論 `EXP5 = a < b`
`EXP4 = a ^ b`
`EXP5 = a < b`
:::

### 撰寫有號/無號 branchless 的實作
無號數的實作和題目的程式碼相同，而有號數和有號數的比較也和無號數之間的比較相同，因此兩者的程式碼實作相同，如以下所示
```c
/** 
 * @fn     - max_uint32
 * @brief  - 找到無號整數比較大的數
 * 
 */
uint32_t max_uint32(uint32_t a, uint32_t b)
{
    return a ^ ((a ^ b) & -(a < b));
}

/** 
 * @fn     - max_int32
 * @brief  - 找到有號整數比較大的數
 * 
 */
int32_t max_int32(int32_t a, int32_t b)
{
    return a ^ ((a ^ b) & -(a < b));
}
```
:::warning
如果是無號和有號的比較則需要特別注意，這邊參考了[你所不知道的 C 語言: bitwise 操作](https://hackmd.io/@sysprog/c-bitwise#%E4%BD%A0%E6%89%80%E4%B8%8D%E7%9F%A5%E9%81%93%E7%9A%84-C-%E8%AA%9E%E8%A8%80-bitwise-%E6%93%8D%E4%BD%9C)，裡頭有更詳細的說明
:::

### 在 Linux 核心原始程式碼中，找出更多類似的實作手法
在[Linux](https://github.com/torvalds/linux)中分別搜尋 `branchless` 及 `branch-free` ，以下為搜尋結果
- 位於[linux/arch/x86/kvm/mmu.h](https://github.com/torvalds/linux/blob/79e06c4c4950be2abd8ca5d2428a8c915aa62c24/arch/x86/kvm/mmu.h)
```c
/*
 * If CPL < 3, SMAP prevention are disabled if EFLAGS.AC = 1.
 *
 * If CPL = 3, SMAP applies to all supervisor-mode data accesses
 * (these are implicit supervisor accesses) regardless of the value
 * of EFLAGS.AC.
 *
 * This computes (cpl < 3) && (rflags & X86_EFLAGS_AC), leaving
 * the result in X86_EFLAGS_AC. We then insert it in place of
 * the PFERR_RSVD_MASK bit; this bit will always be zero in pfec,
 * but it will be one in index if SMAP checks are being overridden.
 * It is important to keep this branchless.
 */
unsigned long smap = (cpl - 3) & (rflags & X86_EFLAGS_AC);
int index = (pfec >> 1) +
        (smap >> (X86_EFLAGS_AC_BIT - PFERR_RSVD_BIT + 1));
bool fault = (mmu->permissions[index] >> pte_access) & 1;
u32 errcode = PFERR_PRESENT_MASK;
```

- 位於 [linux/include/linux/if_vlan.h](https://github.com/torvalds/linux/blob/dcb85f85fa6f142aae1fe86f399d4503d49f2b60/include/linux/if_vlan.h)
```c
/**
 * __vlan_hwaccel_copy_tag - copy hardware accelerated VLAN info from another skb
 * @dst: skbuff to copy to
 * @src: skbuff to copy from
 *
 * Copies VLAN information from @src to @dst (for branchless code)
 */
static inline void __vlan_hwaccel_copy_tag(struct sk_buff *dst, const struct sk_buff *src)
{
	dst->vlan_present = src->vlan_present;
	dst->vlan_proto = src->vlan_proto;
	dst->vlan_tci = src->vlan_tci;
}
```
:::warning
上述程式碼看起來只是將 `src` 的資料複製到 `dst` ，從註解可以看到這樣的動作是要達到 branchless
:::

---
## 測驗 `3`
:::success
延伸問題:
- [x] 解釋上述程式運作原理;
- [ ] 在 x86_64 上透過 `__builtin_ctz` 改寫 GCD ，分析對效能的提升;
- [ ] Linux 核心中也內建 GCD (而且還不只一種實作)，例如 [lib/math/gcd.c](https://github.com/torvalds/linux/blob/master/lib/math/gcd.c)，請解釋其實作手法和探討在核心內的應用場景。
:::

### 程式運作原理
首先分析以下程式碼，並回答 `COND` 及 `RET`
```c
#include <stdint.h>
uint64_t gcd64(uint64_t u, uint64_t v)
{
    if (!u || !v) return u | v;
    int shift;
    for (shift = 0; !((u | v) & 1); shift++) {
        u /= 2, v /= 2;
    }
    while (!(u & 1))
        u /= 2;
    do {
        while (!(v & 1))
            v /= 2;
        if (u < v) {
            v -= u;
        } else {
            uint64_t t = u - v;
            u = v;
            v = t;
        }
    } while (COND);
    return RET;
}
```

從老師附上的 GCD 演算法，讓我能更方便的拆解整個程式碼

> 1. If both x and y are 0, gcd is zero gcd(0,0) = 0
> 2. gcd(x,0) = x and gcd(0,y) = y because everything divides 0

上述敘述可以對應題目的程式碼
```c
if (!u || !v) return u | v;
```

> 3. If x and y are both even, gcd(x,y) = 2∗gcd(x/2,y/2) because 2 is a common divisor. Multiplication with 2 can be done with bitwise shift operator.

上述敘述表達如果兩數都是偶數的話，則可以先將公因數 `2` 提取出來，題目所對應的程式碼如下
```c
for (shift = 0; !((u | v) & 1); shift++) {
    u /= 2, v /= 2;
}
```

> 4. If x is even and y is odd, gcd(x,y) = gcd(x/2,y)
> 5. On similar lines, if x is odd and y is even, then gcd(x,y) = gcd(x,y/2). It is because 2 is not a common divisor.

上述敘述則表達，如果還有兩數還有任一個是 `2` 的倍數的話，可以直接把該數的因數 `2` 除掉，且結果不會改變，對應的題目程式碼如下
```c
while (!(u & 1))
        u /= 2;
    do {
        while (!(v & 1))
            v /= 2;
```

最後則是輾轉相除法的實作，利用連續的減法直到餘數為 0
```c=
    if (u < v) {
        v -= u;
    } else {
        uint64_t t = u - v;
        u = v;
        v = t;
    }
} while (COND);
return RET;
```

分析到這，大致已經了解整個程式邏輯，首先 `COND` 為結束迴圈的條件，從上述的第 2 行和第 6 行，可以看到以下的特性
- 如果 `u` < `v` ， 則 `v -= u`
- 如果 `u` &geq; `v` ，則 `v = u - v`
由此可知 `v` 為每次迭代所產生的餘數，因此可以得知 `COND = v`
而 `u` 就是最大公因數，還要記得前面共同的公因數 2^shift^ ，因此可以得知 `RET = u << shift`

---
## 測驗 `4`
:::success
延伸問題:
- [x] 解釋上述程式運作原理，並舉出這樣的程式碼用在哪些真實案例中;
- [ ] 設計實驗，檢驗 `ctz/clz` 改寫的程式碼相較原本的實作有多少改進？應考慮到不同的 [bitmap density](http://www.vki.com/2013/Support/docs/vgltools-3.html);
- [ ] 思考進一步的改進空間;
  閱讀 [Data Structures in the Linux Kernel](https://0xax.gitbooks.io/linux-insides/content/DataStructures/linux-datastructures-3.html) 並舉出 Linux 核心使用 bitmap 的案例;
:::

### 程式運作原理
分析以下程式碼
```c
#include <stddef.h>
size_t improved(uint64_t *bitmap, size_t bitmapsize, uint32_t *out)
{
    size_t pos = 0;
    uint64_t bitset;
    for (size_t k = 0; k < bitmapsize; ++k) {
        bitset = bitmap[k];
        while (bitset != 0) {
            uint64_t t = EXP6;
            int r = __builtin_ctzll(bitset);
            out[pos++] = k * 64 + r;
            bitset ^= t;                           
        }
    }
    return pos;
}
```
:::info
思考思路
- 這題的思考主要是來自題目的提示
  > 其中第 9 行的作用是找出目前最低位元的 1，並紀錄到 t 變數。

- 得知題目是要找到目前最低位元的 `1` ，接著參考老師給的連結 [Introduction to Low Level Bit Hacks](https://catonmat.net/low-level-bit-hacks) ，裡頭剛好有提到方法，如以下所示
  > Bit Hack #7. Isolate the rightmost 1-bit.
  >  y = x & (-x)
  ```shell
      10111100  (x)
    & 01000100  (-x)
      --------
      00000100
  ```
- 由上述特性，可以得到 `EXP6 = bitset & -bitset`
:::

接著可以開始分析程式碼，最關鍵的部份如下
```c
while (bitset != 0) {
    // 將最低位元為 1 的數值儲存到 t
    uint64_t t = bitset & -bitset;
    // 回傳由低位往高位遇上連續多少個 0 才碰到 1
    int r = __builtin_ctzll(bitset);
    // 將計算出的位置存到 out 裡
    out[pos++] = k * 64 + r;
    // 把最低位元為 1 的 bit 改為 0
    bitset ^= t;
}
```

這邊使用 `uint64_t bitmap[1] = 255` 做測試
```c
int main(void)
{
    uint64_t bitmap[1] = {0xFF};
    uint32_t* out = malloc(8 * sizeof(uint32_t));
    size_t pos = improved(bitmap, 1, out);
    for(size_t i = 0; i < pos; i++) {
        printf("%d\n", *(out + i));
    }
    free(out);
    return 0;
}
```

以下為結果，可以看到 `255` 為 `1` 的 bit 其位置都被儲存了起來，同時也可以得知，如果一個 bitmap 的 `0` 越多，則 `improve` 的執行會越快，呼應題目的敘述
> 若 bitmap 越鬆散 (即 1 越少)，於是 improved 的效益就更高。
```shell
gcc -O1 -g -Wall -Werror -IInclude -o problem4.out quiz2/problem4.c -lm
./problem4.out
0
1
2
3
4
5
6
7
```

### 相關應用
根據老師給的連結 [bit array](https://en.wikipedia.org/wiki/Bit_array) ，這邊可以找到一些應用
- [Priority queue](https://en.wikipedia.org/wiki/Priority_queue)
  > Bit arrays are used for priority queues, where the bit at index k is set if and only if k is in the queue; this data structure is used, for example, by the Linux kernel, and benefits strongly from a find-first-zero operation in hardware.

在 Linux kernel 搜尋 bitmap 後，可以找到一些和 bitmap 有關的檔案，其中有一個標頭檔都是 bitmap 的應用 ，位於 [linux/include/linux/bitmap.h](https://github.com/torvalds/linux/blob/40c843218f11625722e9a7c3ced81a83b95ecf05/include/linux/bitmap.h) ，以下節錄一些程式碼
```c
 *  bitmap_zero(dst, nbits)                     *dst = 0UL
 *  bitmap_fill(dst, nbits)                     *dst = ~0UL
 *  bitmap_copy(dst, src, nbits)                *dst = *src
 *  bitmap_and(dst, src1, src2, nbits)          *dst = *src1 & *src2
 *  bitmap_or(dst, src1, src2, nbits)           *dst = *src1 | *src2
 *  bitmap_xor(dst, src1, src2, nbits)          *dst = *src1 ^ *src2
 *  bitmap_andnot(dst, src1, src2, nbits)       *dst = *src1 & ~(*src2)
 *  bitmap_complement(dst, src, nbits)          *dst = ~(*src)
 *  bitmap_equal(src1, src2, nbits)             Are *src1 and *src2 equal?
 *  bitmap_intersects(src1, src2, nbits)        Do *src1 and *src2 overlap?
 *  bitmap_subset(src1, src2, nbits)            Is *src1 a subset of *src2?
 *  bitmap_empty(src, nbits)                    Are all bits zero in *src?
 *  bitmap_full(src, nbits)                     Are all bits set in *src?
 *  bitmap_weight(src, nbits)                   Hamming Weight: number set bits
 *  bitmap_set(dst, pos, nbits)                 Set specified bit area
 *  bitmap_clear(dst, pos, nbits)               Clear specified bit area
```

---
## 測驗 `5`
:::success
延伸問題:
- [x] 解釋上述程式碼運作原理，指出其中不足，並予以改進
  例如，判斷負號只要寫作 `bool isNegative = numerator < 0 ^ denominator < 0;`
  搭配研讀 [The simple math behind decimal-binary conversion algorithms](https://indepth.dev/posts/1019/the-simple-math-behind-decimal-binary-conversion-algorithms)
- [ ] 在 Linux 核心原始程式碼的 mm/ 目錄 (即記憶體管理) 中，找到特定整數比值 (即 fraction) 和 bitwise 操作的程式碼案例，予以探討和解說其應用場景
:::
參考[166. Fraction to Recurring Decimal](https://leetcode.com/problems/fraction-to-recurring-decimal/)

### 程式運作原理
首先，以下為題目用到儲存餘數的 linked list
```c
/* Rem 結構 */
struct rem_node {
    int key;   // 紀錄餘數
    int index; // 紀錄位數
    struct list_head link;
};
```

程式碼主要架構是將所有的節點接在 hash table ，這樣在搜尋節點時速度較快
```c
// 在這裡建立 hash table
size = 1333;
// 開啟大小為 size 的 hash table
struct list_head *heads = malloc(size * sizeof(*heads));
for (int i = 0; i < size; i++)
    INIT_LIST_HEAD(&heads[i]);
```

接著在每次迭代的時候會先找餘數是否已經重複，如以下程式碼，其中 `pos` 表示這是第幾位小數
```c
int pos = find(heads, size, remainder);
// 出現無窮小數的情況
if (pos >= 0) {
```

如果沒有重複餘數的話，直接創一個資料並加在 hash table
```c
struct rem_node *node = malloc(sizeof(*node));
node->key = remainder; // 紀錄餘數
node->index = i; // 紀錄位數

list_add(&node->link, &heads[remainder % size]);
```

最後附上完整的程式碼分析結果
```c
/* Rem 結構 */
struct rem_node {
    int key;   // 紀錄餘數
    int index; // 紀錄小數點位數
    struct list_head link;
};

/** 
 * @fn     - find
 * @brief  - 尋找資料 key 有沒有存在資料節點 rem_node 上
 * 
 * @attention 函式邏輯
 * 1. 利用 hash function 算出 hash table (hash) 的位置，這邊的 hash function 為 key % size
 * 2. 走訪整個 heads[hash] 接著的 linked list ，尋找資料 num 有沒有存在
 * 3. 如果存在則回傳該節點成員 index ，沒有則回傳 -1
 */
static int find(struct list_head *heads, int size, int key)
{
    struct rem_node *node;
    // 利用 hash function 算出 hash table (hash) 的位置
    int hash = key % size;
    // 走訪整個 heads[hash] 接著的 linked list，尋找資料 num 有沒有存在
    list_for_each_entry (node, &heads[hash], link) {
        if (key == node->key)
            // 如果存在則回傳該節點成員 index
            return node->index;
    }
    // 沒有則回傳 -1
    return -1;
}

/** 
 * @fn     - fractionToDecimal
 * @brief  - 回傳兩整數除完的數值 (利用字串表示小數)
 * 
 * @attention 函式邏輯
 * 1. 如果 denominator 為 0，回傳 '/0'
 * 2. 如果 numerator 為 0，回傳 "0"
 * 3. 如果 denominator 或 numerator 有負數，則先變回正數
 * 4. 先做第一次的除法，處理整數位，如果沒有餘數則直接回傳
 * 5. 開啟大小為 size 的 hash table
 * 6. 開始進行除法計算
 * 7. 如果整除則會離開迴圈，並把 decimal 的資料複製給 p
 * 8. 如果出現無限迴圈會進入 if(pos >= 0)
 */
char *fractionToDecimal(int numerator, int denominator)
{
    int size = 1024;
    char *result = malloc(size);
    char *p = result;
    
    // 如果 denominator 為 0，回傳 '/0'
    if (denominator == 0) {
        result[0] = '\0';
        return result;
    }
    
    // 如果 numerator 為 0，回傳 "0"
    if (numerator == 0) {
        result[0] = '0';
        result[1] = '\0';
        return result;
    }

    /* using long long type make sure there has no integer overflow */
    long long n = numerator;
    long long d = denominator;

    /* deal with negtive cases */
    if (n < 0)
        n = -n;
    if (d < 0)
        d = -d;

    bool sign = (float) numerator / denominator >= 0;
    if (!sign)
        *p++ = '-';
    
    // 先做第一次的除法，處理整數位
    long long remainder = n % d;
    long long division = n / d;

    sprintf(p, "%ld", division > 0 ? (long) division : (long) -division);
    if (remainder == 0)
        return result;

    p = result + strlen(result);
    *p++ = '.';

    /* Using a map to record all of reminders and their position.
     * if the reminder appeared before, which means the repeated loop begin,
     */
    char *decimal = malloc(size);
    memset(decimal, 0, size);
    char *q = decimal;

    size = 1333;
    // 開啟大小為 size 的 hash table
    struct list_head *heads = malloc(size * sizeof(*heads));
    for (int i = 0; i < size; i++)
        INIT_LIST_HEAD(&heads[i]);

    // 開始進行除法計算
    for (int i = 0; remainder; i++) {
        int pos = find(heads, size, remainder);
        // 出現無窮小數的情況
        if (pos >= 0) {
            // 先加上不是循環小數的部份
            while (pos-- > 0)
                *p++ = *decimal++;
            *p++ = '(';
            // 加上循環小數的部份 用 "()" 括住
            while (*decimal != '\0')
                *p++ = *decimal++;
            *p++ = ')';
            *p = '\0';
            return result;
        }
        struct rem_node *node = malloc(sizeof(*node));
        node->key = remainder; // 紀錄餘數
        node->index = i; // 紀錄位數

        list_add(&node->link, &heads[remainder % size]);
        *q++ = (remainder * 10) / d + '0'; // 新的小數點位數
        remainder = (remainder * 10) % d;  // 新的餘數
    }
    // 已經整除
    strcpy(p, decimal);
    return result;
}
```

### 可改進的地方
首先發現判斷是否為負數的程式碼使用了 branch ，這邊的想法是不管怎麼樣都要使用正數轉換，不如就直接用絕對值的方式做修改，參考[以前學員的筆記](https://hackmd.io/@RinHizakura/HJ0rPhxyD)
```diff
/* deal with negtive cases */
- if (n < 0)
-     n = -n;
- if (d < 0)
-     d = -d;
+ int mask = n >> 31;
+ n = (n + mask) ^ mask;
+ mask = d >> 31;
+ d = (d + mask) ^ mask;
```

接著是延伸問題有提示到的部份
> 判斷負號只要寫作 `bool isNegative = numerator < 0 ^ denominator < 0;`
```diff
- bool sign = (float) numerator / denominator >= 0;
+ bool sign = (numerator < 0) ^ (denominator < 0);
```

最後就是記憶體釋放的問題，這邊實作了函式 `free_ht()` 用來釋放 hash table
```c
static void free_ht(struct list_head *heads, int size)
{
    struct rem_node *node, *next;
    for (int i = 0; i < size; i++) {
        list_for_each_entry_safe (node, next, &heads[i], link) {
            list_del(&node->link);
            free(node);
        }
    }
    free(heads);
}
```

接著修改函式 `fractionToDecimal()`
```diff
    // 開始進行除法計算
    for (int i = 0; remainder; i++) {
        int pos = find(heads, size, remainder);
        // 出現無窮小數的情況
        if (pos >= 0) {
+           q = decimal;
            // 先加上不是循環小數的部份
            while (pos-- > 0)
                *p++ = *decimal++;
            *p++ = '(';
            // 加上循環小數的部份 用 "()" 括住
            while (*decimal != '\0')
                *p++ = *decimal++;
            *p++ = ')';
            *p = '\0';
+           free_ht(heads, size);
+           free(q);
            return result;
        }
        struct rem_node *node = malloc(sizeof(*node));
        node->key = remainder; // 紀錄餘數
        node->index = i; // 紀錄位數

        list_add(&node->link, &heads[remainder % size]);
        *q++ = (remainder * 10) / d + '0'; // 新的小數點位數
        remainder = (remainder * 10) % d;  // 新的餘數
    }
    // 已經整除
    strcpy(p, decimal);
+   free_ht(heads, size);
+   free(decimal);
    return result;
}
```
至於 `result = malloc(size)` 的部份，在主函式 `main()` 進行釋放記憶體的動作
```c
void Comput_and_Display(int numerator, int denominator)
{
    char* res = fractionToDecimal(numerator, denominator);
    printf("res = %s\n", res);
    free(res);
}

int main(void)
{
    Comput_and_Display(1, 2);
    Comput_and_Display(4, 333);
    return 0;
}
```

以下為輸出結果，使用的範例參考 [166. Fraction to Recurring Decimal](https://leetcode.com/problems/fraction-to-recurring-decimal/)
```shell
gcc -O1 -g -Wall -Werror -IInclude -o problem5.out quiz2/problem5.c -lm
./problem5.out
res = 0.5
res = 0.(012)
```

---
## 測驗 `6`
:::success
延伸問題:
- [x] 解釋上述程式碼運作原理
- [ ] 在 Linux 核心原始程式碼中找出 [`__alignof__`](https://gcc.gnu.org/onlinedocs/gcc/Alignment.html) 的使用案例 2 則，並針對其場景進行解說
- [ ] 在 Linux 核心源使程式碼找出 `ALIGN`, `ALIGN_DOWN`, `ALIGN_UP` 等巨集，探討其實作機制和用途，並舉例探討 (可和上述第二點的案例重複)。思索能否對 Linux 核心提交貢獻，儘量共用相同的巨集
:::
### 程式運作原理
首先解析以下的巨集
```c
#define ALIGNOF(t) \
    ((char *)(&((struct { char c; t _h; } *)0)->M) - (char *)X)
```
1. `((struct { char c; t _h; } *)0` 表示將地址 `0x0` 轉型成 `(struct { char c; t _h; } *)` 的型態
2. `(&((struct { char c; t _h; } *)0)->M)` 表示取得成員 `M` 的地址 
3. `(char *)(&((struct { char c; t _h; } *)0)->M)` 將 `M` 的地址轉型成 `(char *)` ，目的在於等等計算時會以 1 為單位做計算
4. `(char *)(&((struct { char c; t _h; } *)0)->M) - (char *)X` 最後減去 `X` ，得到 alignment

這邊的話，`M` 很明顯不是 `c` 就是 `_h` ，由於這邊如果是選 `c` 的話， `(char *)(&((struct { char c; t _h; } *)0)->c` 這整串永遠都會是 `0` ，因此答案只能是 `_h`

得知了 `M` 為 `_h` ， 我們可以得知 `(char *)(&((struct { char c; t _h; } *)0)->_h)` ，為成員 `_h` 的地址，如果要計算需要的 alignment ，只要將該地址減去 `struct` 的 base address 即可，接著，可以看到巨集以 `0x0` 作為 `struct` 的 base address ，因此 `X` 可以合理推論為 `0`
> `M = _h`
> `X = 0`

---
## 測驗 `7`
:::success
延伸問題:
- [ ] 解釋上述程式運作原理並評估 `naive.c` 和 `bitwise.c` 效能落差
  - 避免 stream I/O 帶來的影響，可將 `printf` 更換為 `sprintf`
- [ ] 分析 [Faster remainders when the divisor is a constant: beating compilers and libdivide](https://lemire.me/blog/2019/02/08/faster-remainders-when-the-divisor-is-a-constant-beating-compilers-and-libdivide/) 一文的想法 (可參照同一篇網誌下方的評論)，並設計另一種 bitmask，如「可被 3 整除則末位設為 1」「可被 5 整除則倒數第二位設定為 1」，然後再改寫 `bitwise.c` 程式碼，試圖運用更少的指令來實作出 branchless;
  - 參照 [fastmod: A header file for fast 32-bit division remainders on 64-bit hardware](https://lemire.me/blog/2019/02/08/faster-remainders-when-the-divisor-is-a-constant-beating-compilers-and-libdivide/)
- [ ] 研讀 [The Fastest FizzBuzz Implementation](https://tech.marksblogg.com/fastest-fizz-buzz.html) 及其 [Hacker News](https://news.ycombinator.com/item?id=29413656) 討論，提出 throughput (吞吐量) 更高的 Fizzbuzz 實作
- [ ] 解析 Linux 核心原始程式碼 [kernel/time/timekeeping.c](https://github.com/torvalds/linux/blob/master/kernel/time/timekeeping.c) 裡頭涉及到除法運算的機制，探討其快速除法的實作 (注意: 你可能要對照研讀 kernel/time/ 目錄的標頭檔和程式碼)
  > 過程中，你可能會發現可貢獻到 Linux 核心的空間，請充分討論
:::

### 程式運作原理
在解題之前可以先大致看一下整個程式運作的流程，首先就從變數 `M3` `M5` 及函式 `is_divisible()` 開始
```c
/** 
 * @fn     - is_divisible
 * @brief  - 回傳是否可以整除，可以回傳 true, 反則回傳 false
 */
static inline bool is_divisible(uint32_t n, uint64_t M)
{
    return n * M <= M - 1;
}

static uint64_t M3 = UINT64_C(0xFFFFFFFFFFFFFFFF) / 3 + 1; // 0x5555555555555556
static uint64_t M5 = UINT64_C(0xFFFFFFFFFFFFFFFF) / 5 + 1; // 0x3333333333333334
```

單從函式 `is_divisible()` 的實作，不好看出為什麼要這麼寫，不過搭配上 `M3` 和 `M5` 的宣告，我們可以得出題目是如何判斷是否整除

`M3` 和 `M5` 的手法一樣，都是用來分別判斷是否能被 `3` 和 `5` 整除
:::info
分析結果
- `M3` 除以 `3` 表示以 `3` 為循環，而 `+1` 是為了要讓乘上 `3` 的倍數時會 overflow ，而產生一個很小的數
- `M5` 除以 `5` 表示以 `5` 為循環，而 `+1` 是為了要讓乘上 `5` 的倍數時會 overflow ，而產生一個很小的數
- 搭配以上兩點，我們可以得知 `is_divisible()` 的實作手法，利用 overflow 判斷是否可以整除
  1. 當 `M` 為 `M3` 時，如果 `n * M` 產生 overflow 時，表示 `n` 可以被 `3` 整除，此時 `n * M` 是一個很小的數，因此 `n * M <= M - 1` 也就成立，回傳 `true`
  2. 當 `M` 為 `M5` 時，如果 `n * M` 產生 overflow 時，表示 `n` 可以被 `5` 整除，此時 `n * M` 是一個很小的數，因此 `n * M <= M - 1` 也就成立，回傳 `true`
  3. 其他的情況則回傳 `false`
:::

接著可以來解題啦，開始分析 `bitwise` 的實作

:::warning
發現這邊的題目有誤
```diff
- strncpy(fmt, &"FizzBuzz%u"[(9 >> div5) >> (KK3)], length);
+ strncpy(fmt, &"FizzBuzz%u"[(8 >> div5) >> (KK3)], length);
```
:::

```c
void bitwise(void)
{
    for (size_t i = 1; i <= 100; i++) {
        uint8_t div3 = is_divisible(i, M3);
        uint8_t div5 = is_divisible(i, M5);
        unsigned int length = (2 << div3) << div5;

        char fmt[9];
        /** 
         * (8 >> div5) >> (div3 << 2) 有四種可能
         * 1, 為 0 時，如果 length 為 8 ，印出 FizzBuzz
         * 2. 為 0 時，如果 length 為 4 ，印出 Fizz
         * 3. 為 4 時，印出 Buzz
         * 4. 為 8 時，此時 fmt = "%u"，並利用 printf(fmt, i) 印出 i (當前數字)
         */
        strncpy(fmt, &"FizzBuzz%u"[(8 >> div5) >> (div3 << 2)], length);
        fmt[length] = '\0';

        printf(fmt, i);
        printf("\n");
    }
}
```

從整個程式邏輯來看可以推論出 `length` 為需要印出的字串長度，加上題目的敘述對應函式的實作
> 整數格式字串 "%u" : 長度為 2 B
> "Fizz" 或 "Buzz" : 長度為 4 B
> "FizzBuzz" : 長度為 8 B
``` 
string literal: "fizzbuzz%u"
        offset:  0   4   8
```

可以間單歸類出下面的表格
| `div3` | `div5` | `length` `(bytes)` | `(8 >> div5) >> (KK3)` |
| - | - | - | - |
| 0 | 0 | 2 | 8 |
| 0 | 1 | 4 | 4 |
| 1 | 0 | 4 | 0 |
| 1 | 1 | 8 | 0 |

有了以上表格可以得到題目的答案

:::warning
`KK1` = `div3`
`KK2` = `div5`
`KK3` = `div3 << 2`
但經過觀察後，發現 `KK1` 和 `KK2` 對調也是可行的
:::
