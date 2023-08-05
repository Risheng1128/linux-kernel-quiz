/* copy modified blocks of source file to destination file efficiently
 * using mmap.
 */

#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <source> <destination>\n", argv[0]);
        return EX_USAGE;
    }

    const char *src_name = argv[1];
    const char *dst_name = argv[2];
    int src_fd, dst_fd;
    struct stat dst_stat = {0};
    off_t src_len, dst_len;

    src_fd = open(src_name, O_RDONLY);
    if (src_fd == -1) {
        perror(src_name);
        return EX_DATAERR;
    }

    dst_fd = open(dst_name, O_RDWR | O_CREAT,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (dst_fd == -1 || fstat(dst_fd, &dst_stat) != 0) {
        perror(dst_name);
        return EX_DATAERR;
    }

    src_len = lseek(src_fd, 0, SEEK_END);
    if (src_len < 0) {
        perror(src_name);
        return EX_DATAERR;
    }

    dst_len = lseek(dst_fd, 0, SEEK_END);
    if (dst_len < 0) {
        perror(dst_name);
        return EX_DATAERR;
    }

    if (dst_len > src_len) {
        printf("Destination file is larger (%zd) than input file (%zd)\n",
               dst_len, src_len);
        return EX_DATAERR;
    }

    const size_t page_size =
        dst_stat.st_blksize > 0 ? dst_stat.st_blksize : BUFSIZ;
    const size_t len = src_len;

    if (ftruncate(dst_fd, len) != 0) {
        perror(dst_name);
        return EX_DATAERR;
    }

    size_t read_count = 0;
    size_t write_count = 0;

    if (len > 0) {
        const uint8_t *src;
        uint8_t *dst;

        src = mmap(NULL, len, PROT_READ, MAP_SHARED, src_fd, 0);
        if (src == NULL ||
            posix_madvise((void *) src, len, POSIX_MADV_SEQUENTIAL) != 0) {
            perror(src_name);
            return EX_UNAVAILABLE;
        }

        dst = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, dst_fd, 0);
        if (dst == NULL ||
            posix_madvise(dst, len, POSIX_MADV_SEQUENTIAL) != 0) {
            perror(dst_name);
            return EX_UNAVAILABLE;
        }

        for (size_t i = 0; i < len; i += page_size) {
            size_t block_size = (len - i) >= page_size ? page_size : (len - i);

            if (memcmp(src + i, dst + i, block_size)) {
                memcpy(dst + i, src + i, block_size);
                write_count += block_size;
            }

            read_count += block_size;
        }

        if (munmap((void *) src, len) != 0) {
            perror(src_name);
            return EX_UNAVAILABLE;
        }

        if (msync(dst, len, MS_SYNC) != 0 || munmap(dst, len) != 0) {
            perror(dst_name);
            return EX_UNAVAILABLE;
        }
    }

    if (close(src_fd) != 0) {
        perror(src_name);
        return EX_UNAVAILABLE;
    }

    if (close(dst_fd) != 0) {
        perror(dst_name);
        return EX_UNAVAILABLE;
    }

    printf("%zu bytes read\n", read_count);
    printf("%zu bytes written\n", write_count);
    return EXIT_SUCCESS;
}