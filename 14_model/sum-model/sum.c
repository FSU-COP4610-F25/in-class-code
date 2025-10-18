#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#include <unistd.h>
#include <time.h>

#define THREAD_COUNT 3
#define LOOP 3
#define RUNS 100000

long sum = 0;



void T_sum(int id) {
    for (int i = 0; i < LOOP; i++) {
        long temp = sum;
        usleep(rand() % 100);  
        temp = temp + 1;
        usleep(rand() % 100);  
        sum = temp;
    }
}
int run_once() {
    sum = 0;

    spawn(T_sum);
    spawn(T_sum);
    spawn(T_sum);

    join();

    thread_reset();  

    return sum;
}

int cmp_desc(const void *a, const void *b) {
    return *(int *)b - *(int *)a;
}

int main() {
    int results[RUNS];

    for (int i = 0; i < RUNS; i++) {
        results[i] = run_once();
    }


    qsort(results, RUNS, sizeof(int), cmp_desc);

    printf("Sorted unique results (from high to low):\n");
    int last = -1;
    for (int i = 0; i < RUNS; i++) {
        if (results[i] != last) {
            printf("%d\n", results[i]);
            last = results[i];
        }
    }

    return 0;
}

