Here you go — short and simple.

# README

## What this program does

* Starts two POSIX threads: `Ta` prints `a`, `Tb` prints `b`.
* Output order is nondeterministic because both threads run concurrently.
* Stop with `Ctrl+C`.

## Build and run

```bash
make
./ab
```

