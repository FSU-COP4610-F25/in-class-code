#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100000000

static long sum = 0;

static void* T_sum(void *arg) {
    (void)arg;
    for (long i = 0; i < N; i++) {
        sum++;
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, T_sum, NULL) != 0) { perror("pthread_create"); return 1; }
    if (pthread_create(&t2, NULL, T_sum, NULL) != 0) { perror("pthread_create"); return 1; }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("sum = %ld\n", sum);
    printf("2*N = %ld\n", 2L * N);
    return 0;
}
