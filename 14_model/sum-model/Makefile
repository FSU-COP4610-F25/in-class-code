# ===== Auto-build and serve HTML in GitHub Codespaces (with venv) =====

# --- config ---
SYS_PY    ?= python3
VENV_DIR  ?= .venv
PY        := $(VENV_DIR)/bin/python
PIP       := $(VENV_DIR)/bin/pip

MOSAIC    ?= mosaic.py
INPUT     ?= sum.py
VIS_MAIN  ?= ./vis/__main__.py
OUT       ?= my_graph.html
FIXED_PORT?= 8000

STAMP     := $(VENV_DIR)/.deps-installed

# --- default: always print guidance even if nothing is rebuilt ---
default: run postmsg

# Create venv (fallback: show hint if venv module missing)
$(VENV_DIR):
	@if $(SYS_PY) -c "import venv" >/dev/null 2>&1; then \
		$(SYS_PY) -m venv $(VENV_DIR); \
	else \
		echo "ERROR: python3-venv is missing. On Debian/Ubuntu run:"; \
		echo "  sudo apt-get update && sudo apt-get install -y python3-venv"; \
		exit 2; \
	fi

# Bootstrap pip in venv and install deps from requirements.txt
$(STAMP): requirements.txt | $(VENV_DIR)
	$(PY) -m ensurepip --upgrade || true
	$(PY) -m pip install --upgrade pip
	$(PIP) install -r requirements.txt
	@touch $(STAMP)

# Build HTML via pipeline using venv python
$(OUT): $(MOSAIC) $(INPUT) $(VIS_MAIN) $(STAMP)
	$(PY) $(MOSAIC) --check $(INPUT) | $(PY) $(VIS_MAIN) > $@
	@echo "OK -> $@"

run: $(OUT)

# Always-printed guidance after `make`
postmsg:
	@echo ""
	@echo "Next steps:"
	@echo "  1) Run:    make serve"
	@echo "  2) In Codespaces, open the Ports panel, find the forwarded port,"
	@echo "     right-click it, choose 'Open in Browser', click my_graph.html."
	@echo "  3) You will see the model checker state transitions in the page."
	@echo ""
	@echo "Useful commands:"
	@echo "  make serve     # start a background server and print the URL"
	@echo "  make serve-fg  # start a foreground server on port $(FIXED_PORT)"
	@echo "  make open      # try to open the browser via 'gp url' if available"
	@echo "  make stop      # stop the background server"

# Serve on a random free port in background; print a clickable URL
serve: $(OUT) $(STAMP)
	@PORT=$$($(PY) -c 'import socket; s=socket.socket(); s.bind(("",0)); p=s.getsockname()[1]; s.close(); print(p)'); \
	echo $$PORT > .server.port; \
	$(PY) -m http.server $$PORT --bind 0.0.0.0 >/dev/null 2>&1 & echo $$! > .server.pid; \
	echo "Serving in background"; \
	echo "URL: http://localhost:$$PORT/$(OUT)"; \
	echo "Open the Ports panel, right-click this port, choose 'Open in Browser'."; \
	echo "Run 'make stop' to stop."

# Try to open browser using Codespaces CLI (gp). Falls back to printing URL.
open:
	@if [ ! -f .server.port ]; then \
		echo "No running server. Run: make serve"; exit 1; \
	fi
	@PORT=$$(cat .server.port); \
	if command -v gp >/dev/null 2>&1; then \
		URL=$$(gp url $$PORT); \
		echo "Opening: $$URL/$(OUT)"; \
		gp preview "$$URL/$(OUT)"; \
	else \
		echo "Open this URL in your browser: http://localhost:$$PORT/$(OUT)"; \
	fi

# Serve on a fixed port in foreground (Ctrl+C to stop)
serve-fg: $(OUT) $(STAMP)
	$(PY) -m http.server $(FIXED_PORT) --bind 0.0.0.0

# Stop background server
stop:
	@if [ -f .server.pid ]; then \
		kill $$(cat .server.pid) 2>/dev/null || true; \
		rm -f .server.pid .server.port; \
		echo "Server stopped."; \
	else \
		echo "No running server."; \
	fi

# Manual dependency install/refresh
install: $(STAMP)

# Clean generated file
clean:
	rm -f $(OUT)

# Remove venv
clean-venv:
	rm -rf $(VENV_DIR)

.PHONY: default run postmsg serve serve-fg open stop install clean clean-venv

# Reuse the graph file produced once by `make list`
GRAPH := .mosaic.json

# Generate the graph once and list distinct outputs
list: $(STAMP)
	$(PY) $(MOSAIC) --check $(INPUT) > $(GRAPH)
	$(PY) collect.py --list < $(GRAPH)
	@echo ""
	@echo "Graph saved to $(GRAPH)."
	@echo "Next:"
	@echo "  make witness TARGET='sum = 3'   # show one path to an output"
	@echo "  make count K=200                # bounded path counting"
	@echo "  make sample RUNS=10000          # Monte Carlo sampling"

# Require the prebuilt graph; if missing, print a hint
_require-graph:
	@if [ ! -f "$(GRAPH)" ]; then \
		echo "Missing $(GRAPH). Run: make list to generate it."; \
		exit 1; \
	fi

# Show one witness path for a target output
witness: $(STAMP) _require-graph
	@if [ -z "$(TARGET)" ]; then echo "Usage: make witness TARGET='sum = 3'"; exit 1; fi
	$(PY) collect.py --list --target "$(TARGET)" < $(GRAPH)

# Bounded walk counting (works even if the graph has cycles)
count: $(STAMP) _require-graph
	$(PY) count_bounded.py --K $${K:-200} < $(GRAPH)

# Monte Carlo sampling to estimate output probabilities
sample: $(STAMP) _require-graph
	$(PY) sample_outputs.py --runs $${RUNS:-5000} --max-steps $${STEPS:-200} < $(GRAPH)

# Clean only the generated graph
clean-graph:
	rm -f $(GRAPH)

# Short help for students
help:
	@echo "make            # build HTML and show next steps (your default)"
	@echo "make list       # generate $(GRAPH) and list distinct outputs"
	@echo "make witness TARGET='sum = 3'   # print one path to that output"
	@echo "make count K=200                # count walks up to K steps"
	@echo "make sample RUNS=10000          # sample outputs via MC"
	@echo "make serve                     # start local HTTP server"
	@echo "make stop                      # stop the server"
