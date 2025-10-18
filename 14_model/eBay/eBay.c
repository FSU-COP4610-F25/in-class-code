#include "thread.h"

unsigned long balance = 100;

void eBay_withdraw(int amount) {
    if (balance >= amount) {
        // Bugs may only manifest on specific timings. Sometimes
        // we reproduce bugs by inserting sleep()s.

        // usleep(1);

        balance -= amount;
    }
}

void T_eBay() {
    eBay_withdraw(100);
}

int main() {
    spawn(T_eBay);
    spawn(T_eBay);
    join();
    printf("balance = %lu\n", balance);
}
