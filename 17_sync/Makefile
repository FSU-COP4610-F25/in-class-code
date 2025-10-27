# Compiler and flags
CC      := gcc
CFLAGS  := -std=c11 -O2 -Wall -Wextra -Wno-unused-parameter -pthread

LDFLAGS := -pthread

# Programs (derived from your file list)
PROGRAMS := condition_variables \
            semaphore \
            semaphore_no_mutex \
            producer_consumer \
            single_condition_variable

SRCS := $(addsuffix .c,$(PROGRAMS))
OBJS := $(SRCS:.c=.o)

.PHONY: all clean format

# Default: build everything
all: $(PROGRAMS)

# Link each program from its .o
%: %.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Compile .c -> .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Optional: format sources if you use clang-format
format:
	clang-format -i $(SRCS) || true

# Clean build outputs
clean:
	rm -f $(OBJS) $(PROGRAMS)
