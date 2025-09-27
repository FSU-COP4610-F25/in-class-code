#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <time.h>

static int speed_factor = 1;  // Adjust to modify speed
static struct timeval start_tv;
static struct timeval real_start_tv;
static int initialized = 0;

// Match the exact signature from /usr/include/sys/time.h
int gettimeofday(struct timeval *tv, void *tz) {
    static int (*real_gettimeofday)(struct timeval *, void *) = NULL;
    
    if (!real_gettimeofday) {
        real_gettimeofday = dlsym(RTLD_NEXT, "gettimeofday");
    }

    if (!initialized) {
        real_gettimeofday(&real_start_tv, NULL);
        start_tv = real_start_tv;
        initialized = 1;
    }

    int ret = real_gettimeofday(tv, tz);

    if (ret == 0) {
        // Compute elapsed time from real start time
        long elapsed_sec = tv->tv_sec - real_start_tv.tv_sec;
        long elapsed_usec = tv->tv_usec - real_start_tv.tv_usec;

        if (elapsed_usec < 0) {
            elapsed_sec--;
            elapsed_usec += 1000000;
        }

        // Apply speed factor
        elapsed_sec *= speed_factor;
        elapsed_usec *= speed_factor;

        if (elapsed_usec >= 1000000) {
            elapsed_sec += elapsed_usec / 1000000;
            elapsed_usec %= 1000000;
        }

        // Return modified time
        tv->tv_sec = start_tv.tv_sec + elapsed_sec;
        tv->tv_usec = start_tv.tv_usec + elapsed_usec;

        if (tv->tv_usec >= 1000000) {
            tv->tv_sec += tv->tv_usec / 1000000;
            tv->tv_usec %= 1000000;
        }
    }

    return ret;
}

