/*
* gdb
* info proc mappings
*/

#include <stdio.h>
#include <stdlib.h>

int main(){

	// malloc(1000000000*8LL);

	printf("%p\n",main);

	int *p = (int *)main;
	int x = *p;
	printf("%x\n",x);

// 0x5a308668f149
// fa1e0ff3

	// int *q = (void*) 0x12345678LL;
	// int y = *q;
	// printf("%x\n",y);

}
