/**
 * @file    - problem2.c
 * @brief   - 函式 fo_fd 接受指定的地址 v，填入一個 fd 結構體，並確保第一個成員的地址得以對 4 個位元組進行向下對齊
 */

struct foo;
  
struct fd {
    struct foo *foo;
    unsigned int flags;
};

enum {
    FOO_DEFAULT = 0,
    FOO_ACTION,                           
    FOO_UNLOCK,
} FOO_FLAGS;

static inline struct fd to_fd(unsigned long v)
{
    return (struct fd){(struct fd *) (v & ~3), v & 3};
}

int main(void)
{
    return 0;
}


