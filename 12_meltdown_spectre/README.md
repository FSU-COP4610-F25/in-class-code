# Implementation

## 1_CacheTime.c

```
gcc 1_CacheTime.c -O0
./a.out
```

## 2_FlushReload.c

```
gcc 2_FlushReload.c -O0
./a.out
```

## 3_ExceptionHandling.c

```
gcc 3_ExceptionHandling.c
./a.out
```

## 4_MeltdownExperiment.c

```
gcc 4_MeltdownExperiment.c
./a.out
```

## 4_MeltdownExperiment.c

```
gcc 4_MeltdownExperiment.c
./a.out
```
## 4_Fake_MeltdownExperiment.c

```
gcc 4_Fake_MeltdownExperiment.c
./a.out
```


## 5_MeltdownAttack.c

```
gcc 5_MeltdownAttack.c
./a.out
```


❌ Why can't we perform a real Meltdown attack anymore (on kernel addresses)?

After the Meltdown vulnerability was publicly disclosed in 2018, operating systems—especially Linux—implemented several mitigation strategies:

🔐 KPTI (Kernel Page Table Isolation) was introduced:

In user mode, the kernel address space is no longer mapped into the page tables.

As a result, user programs cannot "see" any kernel memory addresses, not even during speculative execution.

Even if a user process tries to access a kernel address like 0xffff..., the CPU will immediately raise a page fault, and speculative execution will not be triggered on that memory access.


## 6_SpectreExperiment.c

```
gcc -march=native 6_SpectreExperiment.c
./a.out
```

## 7_SpectreAttack.c

```
gcc -march=native 7_SpectreAttack.c
./a.out
```