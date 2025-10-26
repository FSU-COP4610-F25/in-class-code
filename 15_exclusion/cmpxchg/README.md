**Spin lock with `cmpxchg`**: Atomic load–store helps us do two things: (1) read a value from memory, and (2) write a value. The instruction still has a stop-the-world effect on multiprocessor systems. So even if many threads call `lock()` at the same time, only one can observe `UNLOCKED` and acquire the lock.

