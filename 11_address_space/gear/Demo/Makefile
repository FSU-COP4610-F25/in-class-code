CFLAGS := -Wall -g

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=)

# Default target
all: $(OBJS)

%: tetris.c
	gcc $(CFLAGS) $< -o $@
	gcc -shared -fPIC -o time_interceptor.so time_interceptor.c -ldl
run:
	LD_PRELOAD=./time_interceptor.so ./tetris
clean:
	rm -f $(OBJS)

.PHONY: all clean
