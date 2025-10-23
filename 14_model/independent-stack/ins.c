#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define N 2

// Each thread writes only its own slot
static volatile char *low[N];
static volatile char *high[N];

static inline void update_range(int t, char *ptr) {
    if (ptr < low[t])  low[t]  = ptr;
    if (ptr > high[t]) high[t] = ptr;
}

// noinline so the recursion depth is visible
__attribute__((noinline))
static void probe(int t, int n) {
    // Local variables live on the stack
    char scratch[64];
    update_range(t, scratch);

    long long kb = (long long)(high[t] - low[t]) / 1024;
    printf("Stack(T%d) >= %lld KB\n", t + 1, kb);

    // Infinite recursion to grow the stack
    probe(t, n + 1);
}

static void* thread_probe(void *arg) {
    int t = *(int*)arg;                 // 0..N-1
    low[t]  = (char*)(uintptr_t)~(uintptr_t)0; // init to max address
    high[t] = (char*)0;                          // init to min address
    probe(t, 0);
    return NULL;
}

int main(void) {
    // Unbuffered stdout so you see lines before a crash
    setvbuf(stdout, NULL, _IONBF, 0);

    pthread_t th[N];
    int ids[N];

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        if (pthread_create(&th[i], NULL, thread_probe, &ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }
    for (int i = 0; i < N; i++) {
        pthread_join(th[i], NULL);
    }
    return 0;
}
