#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <emmintrin.h>
#include <x86intrin.h>

// Define a large array with elements spaced 4096 bytes apart (to avoid prefetching)
uint8_t array[10 * 4096];

int main(int argc, const char **argv) {
    int junk = 0;
    uint64_t time1, time2;
    volatile uint8_t *addr;
    int i;

    // Step 1: Initialize the array to ensure it is mapped into RAM
    for (i = 0; i < 10; i++) {
        array[i * 4096] = 1;
    }

    // Step 2: Flush all array elements from the CPU cache
    for (i = 0; i < 10; i++) {
        _mm_clflush(&array[i * 4096]);
    }

    // Step 3: Access specific elements (these will be loaded into the cache)
    array[3 * 4096] = 100;
    array[7 * 4096] = 200;

    // Step 4: Measure access time to each array element
    for (i = 0; i < 10; i++) {
        addr = &array[i * 4096];

        // Read time before memory access
        time1 = __rdtscp(&junk);

        // Access the memory location (loads into register)
        junk = *addr;

        // Read time after memory access and calculate the difference
        time2 = __rdtscp(&junk) - time1;

        // Print access time in CPU cycles
        printf("Access time for array[%d * 4096]: %d CPU cycles\n", i, (int)time2);
    }

    return 0;
}
