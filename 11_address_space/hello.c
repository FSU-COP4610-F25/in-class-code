/*
gcc -c hello.c
ld hello.o
./a.out; echo $?
python3 -c "print(bin(5370)[2:])"
python3 -c "print(int('11111010', 2))"
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>


int main(){
// void _start() {
	// printf("Hello, World!\n");
	// while(1);
	syscall(SYS_exit,5370);

}
