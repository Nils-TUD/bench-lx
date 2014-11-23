#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static unsigned do_get_cycles(void) {
    unsigned val;
    syscall(336, &val);
    return val;
}

static char *buffer;

int main(int argc, char **argv) {
    if(argc < 4) {
        fprintf(stderr, "Usage: %s <file> <size> <bufsize>\n", argv[0]);
        exit(1);
    }

    unsigned start1 = do_get_cycles();
    int fd = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT);
    if(fd == -1) {
	    perror("open");
	    return 1;
    }
    unsigned start2 = do_get_cycles();

    size_t total = atoi(argv[2]);
    size_t bufsize = atoi(argv[3]);
    buffer = malloc(bufsize);
    if(buffer == NULL) {
	    perror("malloc");
	    return 1;
    }
    size_t pos = 0;
    for(; pos < total; pos += bufsize) {
    	write(fd, buffer, bufsize);
    }

    unsigned end1 = do_get_cycles();
    close(fd);
    unsigned end2 = do_get_cycles();

    printf("Wrote %zu bytes\n", total);
    printf("Total time: %u\n", end2 - start1);
    printf("Open time: %u\n", start2 - start1);
    printf("Write time: %u\n", end1 - start2);
    printf("Close time: %u\n", end2 - end1);
    return 0;
}