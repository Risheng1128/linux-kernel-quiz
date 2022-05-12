#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static void file_execution(size_t size, char *elf, char **f_file, char **envp)                       
{
    char e_elf[256];
    int des = memfd_create("targ_file", FD_CLOEXEC);
    write(des, elf, size);
    sprintf(e_elf, "/proc/self/fd/%u", des);
    execve(e_elf, f_file, envp);
}

int main(int argc, char **argv, char **envp)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int des = open(argv[1], O_RDONLY);
    struct stat l_stat;
    fstat(des, &l_stat);
    char *elf = malloc(l_stat.st_size);
    read(des, elf, l_stat.st_size);
    char *str = memmem(elf, l_stat.st_size, "Hello!", 6);
    str[0] = 'Y', str[5] = 'w';
    file_execution(l_stat.st_size, elf, &argv[1], envp);
    return EXIT_SUCCESS;
}