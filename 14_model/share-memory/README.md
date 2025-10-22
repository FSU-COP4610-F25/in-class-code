# README — Shared vs Local Variables with POSIX Threads

## Overview

This demo starts `NTHREAD=10` threads. Each thread prints a message.
You will compare a **global `x`** (shared by all threads) with a **local `x`** (one copy per thread).

## Build and run

```bash
make
./shm
```

## What the code does

* `pthread_create` launches 10 threads.
* Each thread sleeps a bit using `usleep(id * 100000)` to stagger outputs.
* Each thread prints one line. The printed character uses `x` and the string `"123456789ABCDEF"`.

## Experiment A — `x` as a **global** variable

* Uncomment the global line `int x = 0;` at the top.
* Comment out the local `int x = 0;` inside `Thello`.
* Now all threads share and update the same `x`.
* Output ordering and the chosen character depend on the race on `x`.
  This shows **shared state** across threads.

## Experiment B — `x` as a **local** variable

* Comment the global `int x = 0;`.
* Keep `int x = 0;` inside `Thello`.
* Each thread has its own `x`, starting at 0.
* The printed character no longer depends on other threads.
  This shows **thread-local state**.

