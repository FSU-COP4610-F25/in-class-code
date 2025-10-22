# Peterson Mutual Exclusion Demo

This program shows Peterson’s algorithm with and without memory barriers. Two threads `T_A` and `T_B` try to enter a critical section. An atomic counter `inside` acts as a sentinel.

## How it works

* `a`, `b`, and `turn` are shared flags of Peterson’s protocol.
* `BARRIER` enforces ordering: `#define BARRIER __sync_synchronize()`.
* In `critical_section()`:

  ```c
  assert(atomic_fetch_add(&inside, +1) == 0); // entering: was 0
  assert(atomic_fetch_add(&inside, -1) == 1); // leaving:  was 1
  ```

  Correct mutual exclusion keeps `inside` between 0 and 1.

## Why barriers matter

Without proper barriers compilers and CPUs can reorder or delay visibility. Both threads can believe it is safe and enter together. The assertions then fail.

## Build and run

Compile the C demo with your `thread.h`:

```bash
make
./peterson
```

No output is expected. If mutual exclusion fails the program aborts due to an `assert`.

## Force a failure for class demo

Disable the barrier, then rebuild:

```c
#undef  BARRIER
#define BARRIER
```

Run again. You will likely see one of:

```
Assertion `atomic_fetch_add(&inside, +1) == 0' failed
Assertion `atomic_fetch_add(&inside, -1) == 1' failed
```

## Tips

* Do not print inside the critical section. I/O may hide the bug.
* Do not compile with `-DNDEBUG`. Assertions must stay enabled.
