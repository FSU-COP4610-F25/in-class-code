# ===== Auto-build and serve HTML in GitHub Codespaces =====

# --- config ---
PY        ?= python3
MOSAIC    ?= mosaic.py
INPUT     ?= sum.py
VIS_MAIN  ?= ./vis/__main__.py
OUT       ?= my_graph.html
FIXED_PORT?= 8000

# --- default ---
default: run

# Build HTML via pipeline
$(OUT): $(MOSAIC) $(INPUT) $(VIS_MAIN)
	$(PY) $(MOSAIC) --check $(INPUT) | $(PY) $(VIS_MAIN) > $@
	@echo "OK -> $@"

run: $(OUT)

# Serve on a random free port in background; print a clickable URL
serve: $(OUT)
	@PORT=$$($(PY) -c 'import socket; s=socket.socket(); s.bind(("",0)); p=s.getsockname()[1]; s.close(); print(p)'); \
	echo $$PORT > .server.port; \
	$(PY) -m http.server $$PORT --bind 0.0.0.0 >/dev/null 2>&1 & echo $$! > .server.pid; \
	echo "Serving in background"; \
	echo "URL: http://localhost:$$PORT/$(OUT)"; \
	echo "Run 'make stop' to stop."

# Serve on a fixed port in foreground (Ctrl+C to stop)
serve-fg: $(OUT)
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

# Clean generated file
clean:
	rm -f $(OUT)

.PHONY: default run serve serve-fg stop clean
