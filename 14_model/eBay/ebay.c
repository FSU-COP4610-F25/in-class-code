#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static unsigned long balance = 100;

static void eBay_withdraw(unsigned long amount) {
    // This function has a data race when called from multiple threads.
    // Two threads can both see balance >= amount, then both subtract.
    if (balance >= amount) {
        // Uncomment to increase the chance of exposing the race:
        usleep(1);
        balance -= amount;
    }
}

static void* T_eBay(void *arg) {
    (void)arg;
    eBay_withdraw(100);
    return NULL;
}

int main(void) {
    // Make stdout unbuffered so you see prints even if the process exits early
    setvbuf(stdout, NULL, _IONBF, 0);

    pthread_t t1, t2;
    if (pthread_create(&t1, NULL, T_eBay, NULL) != 0) { perror("pthread_create"); return 1; }
    if (pthread_create(&t2, NULL, T_eBay, NULL) != 0) { perror("pthread_create"); return 1; }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("balance = %lu\n", balance);
    return 0;
}
