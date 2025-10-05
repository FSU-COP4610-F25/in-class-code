# Shared Library Test

Launch 1000 instances of a function whose code size is 100 MB. Observe system memory usage to test whether multiple independently started processes keep only one copy of the shared library’s code.

```bash
make
./bloat
./run
ps ax
```

