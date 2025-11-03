CFLAGS := -O2 -g -I. -fsanitize=thread

race: race.c Makefile
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f race
