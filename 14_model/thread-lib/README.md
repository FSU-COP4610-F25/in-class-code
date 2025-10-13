**Minimal Thread Library:** In this minimal library we wrap POSIX threads and provide thread-management APIs. `spawn(fn)` creates a new thread that runs function `fn`. `join()` waits for all running threads to finish. With these two APIs we can use the system’s multiprocessor resources. Threads can be scheduled on different processors to run in parallel.

