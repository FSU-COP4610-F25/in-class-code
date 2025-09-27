# Project 1: Funny Little Executable (FLE)

## Introduction
FLE (Funny Little Executable) is a custom binary file format designed to support (static) linking and loading. We have developed a simple compiler, linker (leveraging `gcc/ld`), and loader for FLE. This format allows position-independent code with readable, writable, and executable memory mappings to be directly loaded and executed.

## Assignment Instructions
As part of your project, you will use FLE to execute `stack.c` in HW3 and perform a buffer overflow attack. The `badfile` you use is also from HW3.

### Steps:
1. **Compile and Run `stack.c` with FLE**: Unlike traditional compilation, FLE does not support direct compilation of `stack.c` using `gcc`. You can use `Makefile` in Demo, but it still needs additional packages and configurations to make it work.
2. **Debug Compilation Issues**: Expect errors during the process. Identify missing dependencies and debug any encountered errors.
3. **Execute the Buffer Overflow Attack**
4. **Analyze the Binary Format**: Understand the numerical representation in FLE and how the binary structure is mapped into memory.

## Report Requirements
Your report should include:
- **Screenshots & Code**: Show errors encountered and the debugging steps taken to resolve them.
- **Exploit Execution**: Demonstrate the buffer overflow attack using FLE.
- **Binary Analysis**: Explain the numerical representation in the FLE format and how it affects execution.

**Good luck, and make sure to document your debugging process thoroughly!**
