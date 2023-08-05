/**
 * @file    - problem11.c
 * @brief   - 實作計算整數開平方根的函式
 */

static inline unsigned long fls(unsigned long word)
{
    int num = 64 - 1;
        
    if (!(word & (~0ul << 32))) {
        num -= 32;
        word <<= 32;
    }
    if (!(word & (~0ul << (64 - 16)))) {
        num -= 16;
        word <<= 16;
    }
    if (!(word & (~0ul << (64 - 8)))) {
        num -= 8;
        word <<= 8;
    }
    if (!(word & (~0ul << (64 - 4)))) {
        num -= 4;
        word <<= 4;
    }   
    if (!(word & (~0ul << (64 - 2)))) {
        num -= 2;
        word <<= 2;
    }   
    if (!(word & (~0ul << (64 - 1))))
        num -= 1;
    return num;
} 

unsigned long i_sqrt(unsigned long x)
{
    unsigned long b, m, y = 0;

    if (x <= 1)
        return x;

    m = 1UL << (fls(x) & ~1UL);
    while (m) {
        b = y + m;
        XXX;

        if (x >= b) {
            YYY;
            y += m;
        }
        ZZZ;
    }

    return y;
}
