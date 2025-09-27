#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define GiB * (1024LL * 1024 * 1024)
#define MiB * (1024LL * 1024)

int main() {
    volatile uint8_t *p = mmap(
        NULL,
        32 MiB,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1, 0
    );

    printf("mmap: %lx\n", (uintptr_t)p);

    if ((intptr_t)p == -1) {
        perror("cannot map");
        exit(1);
    }

    *(p + 2 MiB) = 1;
    *(p + 4 MiB) = 2;
    *(p + 7 MiB) = 3;
    printf("Read get: %d\n", *(p + 4 MiB));
    printf("Read get: %d\n", *(p + 6 MiB));
    printf("Read get: %d\n", *(p + 7 MiB));
}
