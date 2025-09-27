/* ------------------------------------------------------------
   GDB Quick Guide for Students  （GDB = GNU Debugger）

   1) Compile with debug info and no optimization:
      gcc -g -O0 2_abc.c 

   2) Start GDB with source view:
      gdb -tui ./a.out

   3) Stop at main and run:
      (gdb) start             # stop at the first

   4) Step and inspect variables:
      (gdb) next              # execute one source line
      (gdb) print a           # do this after 'int a = 1;' has executed
      (gdb) next
      (gdb) print b
      (gdb) next
      (gdb) print c           # do this after 'c = a + b;' has executed
      (gdb) info locals       # list all local variables
      # If you get "No symbol 'a'", keep stepping until the variable is in scope.

   5) Continue and quit:
      (gdb) continue
      (gdb) quit

    6) Compile with debug info and optimization:
      gcc -g -O2 2_abc.c 
   ------------------------------------------------------------ */

#include <stdio.h>

int main()
{
    int a = 1;
    int b = 1;
    int c = a + b;

    printf("%d + %d = %d\n", a, b, c);
}