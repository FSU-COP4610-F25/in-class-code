// Try compiling this program:
//
//     gcc 1_helloworld.c
//
// It will generate an executable file named: a.out
//
// 👇 Question: What does the "a" in "a.out" stand for?
// (Hint: it's a historical default name used in Unix)
//
// You can also specify your own output name:
//
//     gcc 1_helloworld.c -o hello   ← compile only
//     ./hello                       ← run it
//
// Or compile & run in one line:
//
//     gcc 1_helloworld.c -o hello && ./hello

#include <stdio.h>
int main()
{
    printf("Hello, World!\n");
    return 0;
}