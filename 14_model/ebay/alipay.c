#include "thread.h"

unsigned long balance = 100;

void Alipay_withdraw(int amount) {
    if (balance >= amount) {
        // Bugs may only manifest on specific timings. Sometimes
        // we reproduce bugs by inserting sleep()s.

        // usleep(1);

        balance -= amount;
    }
}

void T_alipay() {
    Alipay_withdraw(100);
}

int main() {
    spawn(T_alipay);
    spawn(T_alipay);
    join();
    printf("balance = %lu\n", balance);
}
