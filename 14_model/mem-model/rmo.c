#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdatomic.h>

int x = 0, y = 0;
atomic_int flag;

#define F1  1
#define F2  2

#define FLAG \
    atomic_load(&flag)
#define FLAG_XOR(val) \
    atomic_fetch_xor(&flag, val)
#define WAIT_FOR(cond) \
    while (!(cond))    \
        ;

__attribute__((noinline))
void write_x_read_y() {
    int y_;
    asm volatile(
        "movl $1, %0;" // x = 1
        "movl %2, %1;" // y_ = y
        : "=m"(x), "=r"(y_)
        : "m"(y)
    );
    printf("%d ", y_);
}

__attribute__((noinline))
void write_y_read_x() {
    int x_;
    asm volatile(
        "movl $1, %0;" // y = 1
        "movl %2, %1;" // x_ = x
        : "=m"(y), "=r"(x_)
        : "m"(x)
    );
    printf("%d ", x_);
}

void* T_1(void* arg) {
    while (1) {
        // Wait until F1 is raised.
        WAIT_FOR((FLAG & F1));
        write_x_read_y();
        // Put F1 down.
        FLAG_XOR(F1);
    }
    return NULL;
}

void* T_2(void* arg) {
    while (1) {
        // Wait until F2 is raised.
        WAIT_FOR((FLAG & F2));
        write_y_read_x();
        // Put F2 down.
        FLAG_XOR(F2);
    }
    return NULL;
}

void* T_flag(void* arg) {
    while (1) {
        x = 0;
        y = 0;
        __sync_synchronize(); // full barrier
        usleep(1);            // + delay

        // Now, x = 0, y = 0, and flag = 0.
        // Both T_1 and T_2 are waiting for their signals.
        assert(FLAG == 0);

        // flag = 3; Both flags are raised.
        FLAG_XOR(F1 | F2);

        // T1 and T2 are ready to go...
        // They will eventually put F1 and F2 down.
        WAIT_FOR(FLAG == 0);

        printf("\n");
        fflush(stdout);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2, tf;

    atomic_init(&flag, 0);

    pthread_create(&t1, NULL, T_1, NULL);
    pthread_create(&t2, NULL, T_2, NULL);
    pthread_create(&tf, NULL, T_flag, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(tf, NULL);

    return 0;
}
