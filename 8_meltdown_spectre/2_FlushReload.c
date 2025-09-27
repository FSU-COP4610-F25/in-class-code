#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <emmintrin.h>
#include <x86intrin.h>

// Create a large array with 256 pages (each page is 4096 bytes)
uint8_t array[256 * 4096];

// Temporary variable to prevent compiler optimizations
int temp;

// The secret value we want to "leak"
char secret = 94; // ASCII '^'

// Threshold to decide whether a memory access is a cache hit
#define CACHE_HIT_THRESHOLD (80)
#define DELTA 1024

// Flush all entries in array[] from the CPU cache
void flushSideChannel() {
    int i;

    // Access every entry to ensure they are mapped into RAM (not copy-on-write)
    for (i = 0; i < 256; i++) {
        array[i * 4096 + DELTA] = 1;
    }

    // Use clflush to evict all cache lines
    for (i = 0; i < 256; i++) {
        _mm_clflush(&array[i * 4096 + DELTA]);
    }
}

// Simulated victim function that accesses memory based on secret
void victim() {
    // This brings one specific array element into the cache
    temp = array[secret * 4096 + DELTA];
}

// Measure access time to each array entry and report cache hits
void reloadSideChannel() {
    int junk = 0;
    uint64_t time1, time2;
    volatile uint8_t *addr;
    int i;

    for (i = 0; i < 256; i++) {
        addr = &array[i * 4096 + DELTA];

        // Time the memory access using rdtscp
        time1 = __rdtscp(&junk);
        junk = *addr;
        time2 = __rdtscp(&junk) - time1;

        // If access time is short enough, we assume it's cached
        if (time2 <= CACHE_HIT_THRESHOLD) {
            printf("array[%d * 4096 + %d] is in cache.\n", i, DELTA);
            printf("The Secret = %d (ASCII '%c')\n", i, i);
        }
    }
}

int main(int argc, const char **argv) {
    flushSideChannel();     // Step 1: Flush cache
    victim();               // Step 2: Victim accesses secret data
    reloadSideChannel();    // Step 3: Reload and detect which value was cached
    return 0;
}
