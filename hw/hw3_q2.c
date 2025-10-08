#include <unistd.h>

// Do not modify other lines. Any extra code = 0 points.

int main() {
    char *const argv[] = {"env", NULL};    // Insert your code inside the curly braces {} 
    char *const envp[] = {"Hello, World!", NULL};    // Insert your code inside the curly braces {} 
    execve("/usr/bin/env", argv, envp);                   // Insert your code inside the parentheses ()
    return 1;
}