**File descriptors**: A file descriptor is a pointer to an operating system object. System calls use this pointer (fd) to determine which OS object the process wants to access. We manage file descriptors with open, close, read/write, lseek, and dup.

```bash
make            
./fd-alloc     
./fd-offset
```