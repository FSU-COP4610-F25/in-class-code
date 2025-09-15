#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

// Define a buffer to save the execution context
static sigjmp_buf jbuf;

// Signal handler for segmentation fault (SIGSEGV)
static void catch_segv() {
    // Jump back to the saved execution point
    siglongjmp(jbuf, 1);
}

int main() {
    // Address of the "secret" kernel data (not accessible from user space)
    unsigned long kernel_data_addr = 0xfb61b000;

    // Register signal handler for SIGSEGV
    signal(SIGSEGV, catch_segv);

    // Set a checkpoint: if SIGSEGV occurs, control jumps back here
    if (sigsetjmp(jbuf, 1) == 0) {
        // Try to read from a kernel address (will likely cause a fault)
        char kernel_data = *(char*)kernel_data_addr;

        // This line will likely NOT be executed due to fault above
        printf("Kernel data at address %lu is: %c\n", 
               kernel_data_addr, kernel_data);
    } 
    else {
        // This block is executed after catching SIGSEGV
        printf("Memory access violation!\n");
    }

    // Continue execution even after segmentation fault
    printf("Program continues to execute.\n");

    return 0;
}
