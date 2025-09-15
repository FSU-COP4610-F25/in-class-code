/*
 * minimal_hello.s - Minimal Assembly Program
 *
 * 1. Compile the program using gcc:
 *    gcc -c 10_miniHello.s
 *
 * 2. Link the object file (output will default to `a.out`):
 *    ld 10_miniHello.o
 *
 * 3. Run the program:
 *    ./a.out
 *
 * 4. Check the size of the generated output file:
 *    ls -l a.out
 *
 * This program prints "Hello, OS World" in red using ANSI color codes and then exits.
 * 
 * mov $1, %rax        # rax = 1 → syscall number for write
 * mov $1, %rdi        # rdi = 1 → first argument: file descriptor (1 = stdout)
 * lea st(%rip), %rsi  # rsi = &st → second argument: pointer to the buffer (string address)
 * mov $ed - st, %rdx  # rdx = ed - st → third argument: number of bytes to write
 * 
 * .global _start
 * This directive makes the label `_start` visible to the linker. It marks the entry point of the program, so execution begins here when the binary is run.
 * 
 * .section .rodata
 * This switches to the read-only data section of the program. Constants such as strings are usually placed here, since they should not be modified at runtime.
 */


    .global _start
    .section .rodata
st:
    .ascii "\033[01;31mHello, OS World!\033[0m\n"
ed:

    .section .text
_start:
    mov     $1, %rax          # sys_write
    mov     $1, %rdi          # fd = 1 (stdout)
    lea     st(%rip), %rsi    # buf = &st
    mov     $ed - st, %rdx    # count = len
    syscall

    mov     $60, %rax         # sys_exit
    xor     %rdi, %rdi        # status = 0
    syscall
