#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdatomic.h>
#include <unistd.h>

#define A 1
#define B 2

// The caveat is: no matter how many times we run this test
// without seeing it fail, we cannot be certain that we have
// inserted sufficient barriers. Understanding the correctness
// of this code is far beyond the scope of this course.
//
// Try commenting/uncommenting the BARRIER line below to see the effect.
#define BARRIER __sync_synchronize()
// #undef  BARRIER
// #define BARRIER

atomic_int inside;
long count = 0;

void critical_section() {
    // We expect mutual exclusion: only one thread can enter.
    assert(atomic_fetch_add(&inside, +1) == 0);

    // Simulate some work in the critical section.
    // Uncomment below if you want to visualize activity.
    // putchar('.');

    assert(atomic_fetch_add(&inside, -1) == 1);
}

volatile int a = 0, b = 0, turn;

void* T_A(void* arg) {
    while (1) {
        a = 1;                    BARRIER;
        turn = B;                 BARRIER;
        while (1) {
            if (!b) break;        BARRIER;
            if (turn != B) break; BARRIER;
        }

        // T_B can't execute critical_section now.
        critical_section();

        a = 0;                    BARRIER;
    }
    return NULL;
}

void* T_B(void* arg) {
    while (1) {
        b = 1;                    BARRIER;
        turn = A;                 BARRIER;
        while (1) {
            if (!a) break;        BARRIER;
            if (turn != A) break; BARRIER;
        }

        // T_A can't execute critical_section now.
        critical_section();

        b = 0;                    BARRIER;
    }
    return NULL;
}

int main() {
    pthread_t tA, tB;

    atomic_init(&inside, 0);

    pthread_create(&tA, NULL, T_A, NULL);
    pthread_create(&tB, NULL, T_B, NULL);

    pthread_join(tA, NULL);
    pthread_join(tB, NULL);

    return 0;
}
