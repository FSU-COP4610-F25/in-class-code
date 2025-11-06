// philosophers_pthread.c
// POSIX-only version (pthread + semaphore), no custom headers.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

static sem_t table;        // limit concurrent diners to N-1 to prevent deadlock
static sem_t avail[N];     // one semaphore per chopstick

static void *philosopher(void *arg) {
    int id = (int)(intptr_t)arg;

    int lhs = (id + N - 1) % N;   // left chopstick index
    int rhs = id % N;             // right chopstick index

    for (;;) {
        // optional: think a bit
        // usleep(1000);

        // enter table (at most N-1 philosophers can try to eat)
        sem_wait(&table);

        // pick left, then right (ordering is safe because of 'table' gate)
        sem_wait(&avail[lhs]);
        printf("+ %d by T%d\n", lhs, id);

        sem_wait(&avail[rhs]);
        printf("+ %d by T%d\n", rhs, id);

        // eat...
        // usleep(1000);

        // put down
        printf("- %d by T%d\n", lhs, id);
        printf("- %d by T%d\n", rhs, id);
        sem_post(&avail[lhs]);
        sem_post(&avail[rhs]);

        // leave table
        sem_post(&table);
    }
    return NULL;
}

int main(void) {
    pthread_t th[N];

    // allow at most N-1 philosophers to contend, which breaks the cycle
    sem_init(&table, 0, N - 1);

    // each chopstick is initially available
    for (int i = 0; i < N; i++) {
        sem_init(&avail[i], 0, 1);
    }

    // spawn N philosophers with 1..N ids (to match your original prints)
    for (int i = 0; i < N; i++) {
        int id = i + 1;
        pthread_create(&th[i], NULL, philosopher, (void *)(intptr_t)id);
    }

    // keep the program alive (they loop forever)
    for (int i = 0; i < N; i++) {
        pthread_join(th[i], NULL);
    }
    return 0;
}
