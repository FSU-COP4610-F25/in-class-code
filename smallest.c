// ------------------------------------------------------------
// Minimal program with your own entry point (_start)
// Use these comments to guide students
//
// 1) If you compile normally, it fails
//    gcc 4_smallest.c -o a.out
//    Linker error: undefined reference to `main`
//    Reason: the default C runtime expects main
//
// 2) Build with your own entry and without the C runtime
//    Option A (dynamic, small if you call no libc):
//      gcc -nostartfiles -Wl,-e,_start 4_smallest.c -o a.out
//    Option B (no C libraries at all, safest for this demo):
//      gcc -nostdlib -Wl,-e,_start 4_smallest.c -o a.out
//    Option C (fully static, self contained):
//      gcc -static -nostdlib -Wl,-e,_start 4_smallest.c -o a.out
//
//    Notes:
//      - `_start` is the process entry symbol
//      - `-Wl,-e,_start` tells the linker to use `_start` as the entry
//      - `-nostartfiles` keeps libc but removes startup files
//      - `-nostdlib` removes both startup files and standard libraries
//
// 3) Debug at the instruction level
//    gdb -tui ./a.out
//    (gdb) layout asm
//    (gdb) starti               # stop at _start
//    (gdb) si                   # step one instruction
//    (gdb) si                   # step into if it calls something
//
// 4) Try a tiny change to observe state
//    Add a volatile variable, then rebuild and step in GDB
//    Example:
//      volatile int value = 42;   // place before the loop
//    You will see a store to memory in the asm window
//
// 5) Pitfalls to show in class
//    - Returning from `_start` has no caller and may crash
//    - Touching invalid memory will cause a segmentation fault
//    - Without `-g` you still can step asm, but there are no source lines
// 
// 6) use "info reg" to observe registers
//    - see how they change as you step instructions
//    - see how they are initialized at program start
//    - see how they are changed by instructions
// ------------------------------------------------------------

// void _start()
// {
//     __asm__("mov$60,%eax\n"  // syscall:exit
//             "xor%edi,%edi\n" // status:0
//             "syscall");
// }

void _start(void)
{
    // Uncomment to observe a store in GDB:
    // volatile int value = 42;

    while (1)
    {
        // infinite loop so the process does not exit
    }
}
