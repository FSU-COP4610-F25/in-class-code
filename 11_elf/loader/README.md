# Static ELF Loader

In the Linux operating system, the `execve` system call can directly load a program, effectively "resetting" the state machine.  
At the same time, we can manually simulate the behavior of `execve`: by mapping the necessary sections of an ELF file into memory and constructing the correct initial stack and registers according to the ABI, we can achieve binary file "loading."

