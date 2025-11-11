// philosophers_cv_no_table.c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define N 5

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cv  = PTHREAD_COND_INITIALIZER;

static bool avail[N] = { [0 ... N-1] = true };

static void *philosopher(void *arg) {
    int id  = (int)(intptr_t)arg;       
    int lhs = (id + N - 1) % N;          
    int rhs = id % N;                  

    for (;;) {

        pthread_mutex_lock(&mtx);
        while (!(avail[lhs] && avail[rhs])) {
            pthread_cond_wait(&cv, &mtx);
        }

        avail[lhs] = false;
        avail[rhs] = false;
        pthread_mutex_unlock(&mtx);



        printf("- %d by T%d\n", lhs, id);
        printf("- %d by T%d\n", rhs, id);
        pthread_mutex_lock(&mtx);
        avail[lhs] = true;
        avail[rhs] = true;
        pthread_cond_broadcast(&cv);   
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main(void) {
    pthread_t th[N];
    for (int i = 0; i < N; i++) {
        int id = i + 1;
        pthread_create(&th[i], NULL, philosopher, (void *)(intptr_t)id);
    }
    for (int i = 0; i < N; i++) pthread_join(th[i], NULL);
    return 0;
}
