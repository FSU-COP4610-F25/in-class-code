mmap: The Linux system uses anonymous mmap to allocate large contiguous memory regions—even at the moment the system call returns, the process may not have received any actual memory. Instead, allocation occurs only upon the first access when a page fault is triggered. AddressSanitizer utilizes mmap to allocate shadow memory, which we can observe using strace.


'''
lsblk
'''