CC := ./cc
LD := ./ld

CFLAGS := -Wall -z execstack -fno-stack-protector -no-pie -Wno-unused-variable -g -Os

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

# Build and link stack (fle)
stack: $(OBJS) tools
	$(LD) $(SRCS:.c=.fle) -o $@
	echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
%.o: %.c tools
	$(CC) $(CFLAGS) $< -o $@

# fle.py is all the binutils!
tools:
	@ln -f -s ../fle.py cc
	@ln -f -s ../fle.py ld
	@ln -f -s ../fle.py exec
	@ln -f -s ../fle.py objdump
	@ln -f -s ../fle.py readfle
	@ln -f -s ../fle.py nm

clean:
	rm -f $(OBJS) $(SRCS:.c=.fle) cc ld exec exec objdump readfle nm stack

.PHONY: all clean tools