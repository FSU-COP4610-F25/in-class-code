# Makefile
PYTHON := python3
OUT    := a.out

.PHONY: all dump run clean

all: $(OUT)

$(OUT): elf-minimal.py
	$(PYTHON) $< > $@
	chmod +x $@

dump: $(OUT)
	objdump -d $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)