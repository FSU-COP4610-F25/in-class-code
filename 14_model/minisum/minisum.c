// xy_flag_pthread.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

static int x = 0, y = 0;
static atomic_int flag;          // initially 0

enum { F1 = 1, F2 = 2 };

#define FLAG        atomic_load(&flag)
#define FLAG_XOR(v) atomic_fetch_xor(&flag, (v))
#define WAIT_FOR(cond) do { while (!(cond)) {} } while (0)

__attribute__((noinline))
static void write_x_read_y(void) {
    int y_;
    asm volatile(
        "movl $1, %0;"   // x = 1
        "movl %2, %1;"   // y_ = y
        : "=m"(x), "=r"(y_)
        : "m"(y)
        : "memory"
    );
    printf("%d ", y_);
}

__attribute__((noinline))
static void write_y_read_x(void) {
    int x_;
    asm volatile(
        "movl $1, %0;"   // y = 1
        "movl %2, %1;"   // x_ = x
        : "=m"(y), "=r"(x_)
        : "m"(x)
        : "memory"
    );
    printf("%d ", x_);
}

static void* T_1(void *arg) {
    (void)arg;
    for (;;) {
        // Wait until F1 is raised.
        WAIT_FOR((FLAG & F1));
        write_x_read_y();
        // Put F1 down.
        FLAG_XOR(F1);
    }
    return NULL;
}

static void* T_2(void *arg) {
    (void)arg;
    for (;;) {
        // Wait until F2 is raised.
        WAIT_FOR((FLAG & F2));
        write_y_read_x();
        // Put F2 down.
        FLAG_XOR(F2);
    }
    return NULL;
}

static void* T_flag(void *arg) {
    (void)arg;
    for (;;) {
        x = 0;
        y = 0;
        atomic_thread_fence(memory_order_seq_cst); // full fence
        usleep(1);                                 // small delay

        // Now, x = 0, y = 0, and flag = 0.
        // Both T_1 and T_2 are waiting for their signals.
        assert(FLAG == 0);

        // Raise both flags (flag ^= (F1|F2)), waking T_1 and T_2.
        FLAG_XOR(F1 | F2);

        // Wait until both threads put their flags down.
        WAIT_FOR(FLAG == 0);

        printf("\n");
        fflush(stdout);
    }
    return NULL;
}

int main(void) {
    // Unbuffered stdout so lines appear even if the program is killed
    setvbuf(stdout, NULL, _IONBF, 0);

    pthread_t t1, t2, tf;
    if (pthread_create(&t1, NULL, T_1, NULL) != 0) { perror("pthread_create t1"); return 1; }
    if (pthread_create(&t2, NULL, T_2, NULL) != 0) { perror("pthread_create t2"); return 1; }
    if (pthread_create(&tf, NULL, T_flag, NULL) != 0) { perror("pthread_create flag"); return 1; }

    // These threads run forever; join will block indefinitely.
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(tf, NULL);
    return 0;
}
