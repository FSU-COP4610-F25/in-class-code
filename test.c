#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    printf("pid=%d ppid=%d\n", (int)getpid(), (int)getppid());
    return 0;
}