#include "thread.h"

#define N 100000000

long sum = 0;

void T_sum() {
    for (int i = 0; i < N; i++) {
        sum++;

        // Won't work even if we force a single-instruction
        // increment.

        // asm volatile(
        //     "incq %0" : "+m"(sum)
        // );
    }
}

int main() {
    spawn(T_sum);
    spawn(T_sum);

    join();

    printf("sum = %ld\n", sum);
    printf("2*n = %ld\n", 2L * N);
}
