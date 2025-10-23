# Auto-venv Makefile with fallbacks for Codespaces

# ---- config ----
PY        ?= python3
VENV_DIR  ?= .venv
PYTHON     = $(VENV_DIR)/bin/python
PIP        = $(VENV_DIR)/bin/pip

MOSAIC    ?= mosaic.py
INPUT     ?= sum.py
VIS_MAIN  ?= ./vis/__main__.py
OUT       ?= my_graph.html

STAMP     := $(VENV_DIR)/.deps-installed

# ---- default ----
default: run

# Create venv with fallbacks:
# 1) python -m venv if available
# 2) pipx + virtualenv if available
# Otherwise, print how to apt-get install python3-venv
$(VENV_DIR):
	@if $(PY) -c "import venv" >/dev/null 2>&1; then \
		echo "[venv] using builtin venv"; \
		$(PY) -m venv $(VENV_DIR); \
	elif command -v pipx >/dev/null 2>&1; then \
		echo "[venv] builtin venv missing; using pipx + virtualenv"; \
		pipx install virtualenv >/dev/null 2>&1 || true; \
		if ! command -v virtualenv >/dev/null 2>&1; then \
			echo "virtualenv not found; please install python3-venv via apt."; \
			exit 2; \
		fi; \
		virtualenv $(VENV_DIR); \
	else \
		echo "ERROR: venv module is missing."; \
		echo "Fix on Debian/Ubuntu:"; \
		echo "  sudo apt-get update && sudo apt-get install -y python3-venv"; \
		exit 2; \
	fi

# Install deps into venv (idempotent)
$(STAMP): requirements.txt | $(VENV_DIR)
	$(PYTHON) -m pip install --upgrade pip
	$(PIP) install -r requirements.txt
	@touch $(STAMP)

# Build HTML using venv python
$(OUT): $(MOSAIC) $(INPUT) $(VIS_MAIN) $(STAMP)
	$(PYTHON) $(MOSAIC) --check $(INPUT) | $(PYTHON) $(VIS_MAIN) > $@
	@echo "OK -> $@"

# Convenience targets
run: $(OUT)

open: $(OUT)
	$(PYTHON) -c 'import webbrowser, pathlib; p=pathlib.Path("$(OUT)").resolve().as_uri(); webbrowser.open(p); print("Opened:", p)'

install: $(STAMP)

clean:
	rm -f $(OUT)

clean-venv:
	rm -rf $(VENV_DIR)

.PHONY: default run open install clean clean-venv
