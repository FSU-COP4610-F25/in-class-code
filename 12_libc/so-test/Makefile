CC := gcc
CFLAGS := -fPIC -O2 -Wall -Wextra

LIB := libbloat.so

all: bloat

$(LIB): bloat.o
	$(CC) -shared -Wl,-soname,$(LIB) -o $@ $^

bloat: main.o $(LIB)
	$(CC) -o $@ main.o -L. -lbloat -Wl,-rpath,'$$ORIGIN'

run: bloat
	LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH ./bloat

clean:
	rm -f *.o $(LIB) bloat

.PHONY: all run clean