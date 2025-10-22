all: sum

CFLAGS := -O2 -I.

sum: sum.c Makefile
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f sum
