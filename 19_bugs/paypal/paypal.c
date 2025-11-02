#include "thread.h"

unsigned long balance = 100;

void Paypal_withdraw(int amount) {
    if (balance >= amount) {
        // Bugs may only manifest on specific timings. Sometimes
        // we reproduce bugs by inserting sleep()s.

        // usleep(1);

        balance -= amount;
    }
}

void T_paypal() {
    Paypal_withdraw(100);
}

int main() {
    create(T_paypal);
    create(T_paypal);
    join();
    printf("balance = %lu\n", balance);
}
