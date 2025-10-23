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

# --- default ---
default: run

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

# Serve on a random free port in background; print a clickable URL
serve: $(OUT) $(STAMP)
	@PORT=$$($(PY) -c 'import socket; s=socket.socket(); s.bind(("",0)); p=s.getsockname()[1]; s.close(); print(p)'); \
	echo $$PORT > .server.port; \
	$(PY) -m http.server $$PORT --bind 0.0.0.0 >/dev/null 2>&1 & echo $$! > .server.pid; \
	echo "Serving in background"; \
	echo "URL: http://localhost:$$PORT/$(OUT)"; \
	echo "Run 'make stop' to stop."

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

.PHONY: default run serve serve-fg stop install clean clean-venv
