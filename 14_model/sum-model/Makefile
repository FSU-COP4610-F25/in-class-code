# Simple Makefile to build HTML via pipeline

PY      ?= python3
MOSAIC  ?= mosaic.py
INPUT   ?= sum.py
VIS     ?= ./vis/__main__.py
OUT     ?= my_graph.html

# Default target
default: $(OUT)

# Build the HTML file
$(OUT): $(MOSAIC) $(INPUT) $(VIS)
	$(PY) $(MOSAIC) --check $(INPUT) | $(PY) $(VIS) > $@
	@echo "OK -> $@"

# Clean generated file
clean:
	rm -f $(OUT)

.PHONY: default clean
