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

uint32_t average4(uint32_t low, uint32_t high)
{
    return high - (high - low) / 2;
}

// uint32_t average2(uint32_t a, uint32_t b)
// {
//     return (a >> 1) + (b >> 1) + (EXP1);
// }

// uint32_t average3(uint32_t a, uint32_t b)
// {
//     return (EXP2) + ((EXP3) >> 1);
// }

int main(void)
{
   uint32_t low = 3, high = 5;
   printf("%x\n", average1(low, high));
}