# Funny Little Executable (FLE)

FLE (Funny Little Executable) is a custom binary file format designed to support (static) linking and loading. We have developed a simple compiler, linker (leveraging `gcc/ld`), and loader for FLE. This format allows position-independent code with readable, writable, and executable memory mappings to be directly loaded and executed.

## Demo 1
```bash
make
./minimal.fle ; echo $?
```

## Demo 2
```bash
./readfle foo.fle
```

## Demo 3
```bash
gcc -E foo.c
gcc -S foo.c
gcc -c foo.c
readelf -a foo.o
```