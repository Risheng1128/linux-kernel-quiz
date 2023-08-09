#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_DIGITS 15
#define LIMIT 1500

typedef unsigned long long ull;

static ull pow10[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
};
static int midindex;

static uint8_t *psieve;
static ull half_max;

#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) __ALIGN_MASK(x, (typeof(x))(a) -1)

enum {
    PRIME = 0,
    COMPOSITE = 1,
};

static inline ull fastpow10(const int n)
{
    if (n < 0 || n > 16) {
        free(psieve);
        printf("n = %d\n", n);
        exit(1);
    }

    return pow10[n];
}

/* isqrt64_tab[k] = isqrt(256 * (k + 65) - 1) for 0 <= k < 192 */
static const uint8_t isqrt64_tab[192] = {
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
    143, 143, 144, 145, 146, 147, 148, 149, 150, 150, 151, 152, 153, 154, 155,
    155, 156, 157, 158, 159, 159, 160, 161, 162, 163, 163, 164, 165, 166, 167,
    167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 175, 176, 177, 178,
    178, 179, 180, 181, 181, 182, 183, 183, 184, 185, 185, 186, 187, 187, 188,
    189, 189, 190, 191, 191, 192, 193, 193, 194, 195, 195, 196, 197, 197, 198,
    199, 199, 200, 201, 201, 202, 203, 203, 204, 204, 205, 206, 206, 207, 207,
    208, 209, 209, 210, 211, 211, 212, 212, 213, 214, 214, 215, 215, 216, 217,
    217, 218, 218, 219, 219, 220, 221, 221, 222, 222, 223, 223, 224, 225, 225,
    226, 226, 227, 227, 228, 229, 229, 230, 230, 231, 231, 232, 232, 233, 234,
    234, 235, 235, 236, 236, 237, 237, 238, 238, 239, 239, 240, 241, 241, 242,
    242, 243, 243, 244, 244, 245, 245, 246, 246, 247, 247, 248, 248, 249, 249,
    250, 250, 251, 251, 252, 252, 253, 253, 254, 254, 255, 255,
};

/* integer square root of a 64-bit unsigned integer */
static ull isqrt(ull x)
{
    if (x == 0)
        return 0;

    int lz = __builtin_clzll(x) & 62;
    x <<= lz;
    uint32_t y = isqrt64_tab[(x >> 56) - 64];
    y = (y << 7) + (x >> 41) / y;
    y = (y << 15) + (x >> 17) / y;
    y -= x < (uint64_t) y * y;
    return y >> (lz >> 1);
}

/* Generates a table of prime numbers up to the maximum having digits.
 * All even numbers (including 2) are ignored. Bit n in the bitmap
 * represents the odd number (2n + 1). This enables the table to be
 * half the size of a naive Sieve of Eratosthenes implementation.
 */
static void generate_sieve(int digits)
{
    ull max = 0;
    for (int count = 0; count < digits; ++count)
        max = max * 10 + 9;

    max = isqrt(max);
    half_max = max >> 1;

    /* We need half the space as multiples of 2 can be omitted */
    int bytes = (max >> 1) + (max & 0x1);

    /* Calculate the actual number of bytes required */
    bytes = (bytes >> 3) + (bytes & 0x1);

    bytes = ALIGN(bytes, 16); /* Align-up to 16-byte */
    psieve = realloc(psieve, bytes);
    if (!psieve) {
        printf("realloc() failed!\n");
        exit(1);
    }
    memset(psieve, 0, bytes);

    /* In psieve bit 0 -> 1, 1 -> 3, 2 -> 5, 3 -> 7 and so on... */
    /* Set the 0th bit representing 1 to COMPOSITE
     */
    psieve[0] |= COMPOSITE << (1 >> 1);

    unsigned char mask = 0x7;
    for (ull n = 3; n <= max; n += 2) {
        if (((psieve[n >> 4] >> ((n >> 1) & mask)) & 0x1) == PRIME) {
            for (ull mul = (n << 1); mul < max; mul += n) {
                /* Skip the evens: there is no representation in psieve */
                if (!(mul & 0x1))
                    continue;

                /* Set offset of mul in psieve */
                psieve[mul >> 4] |= COMPOSITE
                                    << ((mul >> 1) & mask); /* bit offset */
            }
        }
    }
}

/* Convert long to ASCII */
static char *ltoa(ull val, int *len)
{
    static char ascbuf[32] = {0};
    int i = 30;

    for (; val && i; --i, val /= 10)
        ascbuf[i] = "0123456789"[val % 10];

    *len = 30 - i;
    return &ascbuf[++i];
}

/* Check if a number (converted to string) is divisible by 3 */
static bool is_divisible_by3(char *buf, int len)
{
    int sum = 0;
    while (--len >= 0)
        sum += buf[len] - '0';

    static uint32_t M3 = UINT32_C(0xFFFFFFFF) / 3 + 1;
    return (sum * M3) <= (M3 - 1);
}

/* Check if a number is prime. */
static bool isprime(const ull val)
{
    if (!(val & 0x1)) /* Test for divisibility by 2 */
        return false;

    ull *pquadbits = (ull *) psieve;
    ull next = 3; /* start at 7 (i.e. 3 * 2 + 1) */

    for (ull quad = ~*pquadbits & ~0b111, prev = 0; prev <= half_max;
         quad = ~*++pquadbits) {
        if (!quad) {
            prev += 64;
            continue;
        }

        while (quad) {
            ull i = EXP1;
            next = prev + i;
            if (!(val % ((next << 1) + 1)))
                return false;
            quad &= EXP2;
        }

        prev += 64;
    }

    return true;
}

/* Generate the next palindrome after a palindrome
 *   123454321 -> 123464321
 *   123494321 -> 123505321
 *   123999321 -> 124000421
 */
static ull getnextpalin(char *buf, const int *len)
{
    midindex = (*len >> 1) - 1;

    /* Handle the case of odd number of digits.
     * If the central digit is 9, reset it to 0 and process adjacent digit,
     * otherwise increment it and return.
     */
    if ((*len & 0x1) == 0x1) {
        if (buf[midindex + 1] - '0' != 9) {
            buf[midindex + 1] += 1;
            return atol(buf);
        }
        buf[midindex + 1] = '0';
    }

    /* Adjust other digits */
    for (; midindex >= 0; --midindex) {
        if (buf[midindex] - '0' != 9) {
            buf[midindex] += 1;
            buf[*len - midindex - 1] += 1;
            return atol(buf);
        }
        buf[midindex] = '0';
        buf[*len - midindex - 1] = '0';
    }

    /* If we have exhausted numbers in *len digits, increase the number of
     * digits and return the first palindrome of the form 10..0..01
     */

    /* The last palin will always be of the form 99...99 */
    return atol(buf) + 2;
}

int main()
{
    int count = LIMIT;
    ull i = 100000000000001;
    int len = 0;
    char *buf = ltoa(i, &len);

    if (len < N_DIGITS) {
        printf("len: %d\n", len);
        exit(1);
    }

    generate_sieve(N_DIGITS);

    /* We started at 1000000000001. */
    while (1) {
        /* If number of digits are even, all palindromes are divisible by 11.
         * Get first palindrome of next odd number of digits.
         */
        if (!(len & 0x1)) {
            i = 1ULL | fastpow10(len);
            buf = ltoa(i, &len);
            continue;
        }

        /* Check if number is divisible by 5 or 3 */
        if ((buf[len - 1] != '5') && !is_divisible_by3(buf, len)) {
            if (isprime(i) && (--count == 0)) {
                printf("%s\n", buf);
                return 0;
            }
        }

        int oldlen = len;
        i = getnextpalin(buf, &len);
        /* Refresh buffer if number of digits has increased */
        if (oldlen != len)
            buf = ltoa(i, &len);
    }

    free(psieve);

    return 0;
}