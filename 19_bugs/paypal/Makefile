all: paypal

# CFLAGS := -O0 -g
CFLAGS := -O0 -g -fsanitize=thread

paypal: paypal.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f paypal
