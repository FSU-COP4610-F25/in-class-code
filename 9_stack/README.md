# Stack Overflow Example and Exploit Guide

## Overview
This directory contains a simple stack overflow example program (`stack.c`) and a corresponding exploit script (`exploit.py`). The goal is to demonstrate how to overwrite the return address on the stack and redirect the program's execution to custom shellcode.

## Files

### stack.c
This C program contains a function `bof(char *str)` which copies data into a vulnerable buffer (size 100 bytes) using `strcpy`. The function `main()` reads 300 bytes from `badfile` and then calls `bof(str)`.

### exploit.py
This Python script generates a malicious file named `badfile` containing:
- A NOP sled (series of 0x90 bytes)
- Shellcode that spawns a shell
- Overwritten return address pointing back into the NOP sled or the shellcode

At a high level, `exploit.py`:
1. Fills a 300-byte array with 0x90 (NOP instructions)
2. Places the shellcode at the end of this buffer
3. Overwrites the saved return address with a pointer that (hopefully) lands somewhere in the NOP sled or directly in the shellcode

## Usage

### Compile the C Program
```bash
make compile
```

Note: Older versions of GCC might also need `-m32` or other flags depending on your environment.

### Generate the Exploit Payload
```bash
python3 exploit.py
```
This will produce:
- `badfile`: The 300-byte binary payload
- A hexdump of `badfile` printed to standard output for debugging

### Run the Vulnerable Program
```bash
./stack
```
If the exploit is successful, the shellcode should spawn a shell. If the exploit fails, you might just see "Segmentation Falut".

## Step-by-Step Verification with GDB

### 1. Launch GDB
```bash
gdb ./stack
```

### 2. Set a Breakpoint and Run
```gdb
(gdb) set disable-randomization on
(gdb) b foo
(gdb) run
```

### 3. Inspect Buffer and Registers
```gdb
(gdb) p/x &buffer            # Address of buffer on the stack
(gdb) p/x $rbp               # Current base pointer
(gdb) p/d $rbp - (long)&buffer
```
The last command shows the bytes between buffer and rbp, helping calculate the correct return address offset.

### 4. Examine Memory Before Overwrites
```gdb
(gdb) x/160b ((char*)&buffer - 8)
```

### 5. Step Through Instructions
```gdb
(gdb) ni
(gdb) ni
(gdb) ni
(gdb) ni
(gdb) ni
```

### 6. Examine Memory After Overwrites
```gdb
(gdb) x/160b ((char*)&buffer - 8)
(gdb) x/300b ((char*)&buffer + 144)
```

### 7. Continue Execution
```gdb
(gdb) continue
```

## Further Tips

### Adjust Offsets
If the shell doesn't appear, carefully adjust:
- The return address offset from the start of the buffer
- The location that your overwritten return address points to (within the NOP sled or directly at your shellcode)