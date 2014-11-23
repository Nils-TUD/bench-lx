#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned do_get_cycles(void) {
    unsigned val;
    syscall(336, &val);
    return val;
}

static void copy(void *dst, const void *src, size_t size) {
    unsigned *d = (unsigned*)dst;
    unsigned *s = (unsigned*)src;
    unsigned *end = s + size / sizeof(unsigned);
    while(s != end)
        *d++ = *s++;
}

static char buffer[1024];

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    unsigned start1 = do_get_cycles();
    int fd = open(argv[1], O_RDONLY);
    off_t total = lseek(fd, 0, SEEK_END);
    void *addr = mmap(NULL, total, PROT_READ, MAP_PRIVATE, fd, 0);
    unsigned start2 = do_get_cycles();

    off_t pos = 0;
    while(pos < total) {
        size_t amount = (size_t)(total - pos) < sizeof(buffer) ? (size_t)(total - pos) : sizeof(buffer);
        copy(buffer, addr + pos, amount);
        pos += amount;
    }

    unsigned end1 = do_get_cycles();

    unsigned start3 = do_get_cycles();
    pos = 0;
    unsigned checksum = 0;
    unsigned *p = (unsigned*)addr;
    unsigned *end = p + total / sizeof(unsigned);
    while(p < end)
        checksum += *p++;
    // while(pos < total) {
    //     size_t amount = (size_t)(total - pos) < sizeof(buffer) ? (size_t)(total - pos) : sizeof(buffer);
    //     copy(buffer, addr + pos, amount);
    //     pos += amount;
    // }

    unsigned end3 = do_get_cycles();

    munmap(addr, total);
    close(fd);
    unsigned end2 = do_get_cycles();

    printf("Read %zu bytes\n", (size_t)pos);
    printf("Total time: %u\n", end2 - start1);
    printf("Open time: %u\n", start2 - start1);
    printf("Read time: %u\n", end1 - start2);
    printf("Read-again time: %u\n", end3 - start3);
    printf("Close time: %u\n", end2 - end3);
    printf("Checksum: %u\n", checksum);
    return 0;
}