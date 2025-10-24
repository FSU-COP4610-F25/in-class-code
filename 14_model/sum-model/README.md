# Concurrent Sum Model — Visualizer and Analyzer

## What this project does

This project explores non-determinism in concurrent programs.
You write a small program (like `sum.py`) that uses a tiny thread API. The tool explores possible schedules, produces a state graph as JSON, and then:

* renders an interactive HTML visualization of the state graph, and
* analyzes all terminal outputs, prints a witness path for a chosen output, and estimates how often each result can appear.

Students can see why `sum` can be smaller than they first expect. They can also get one concrete schedule that leads to a given result.

---

## Repository layout

```
sum-model/
├── mosaic.py             # Explorer: builds the full state graph (JSON)
├── vis/                  # HTML generator (Jinja2 template based)
│   └── __main__.py
├── my_graph.html         # Generated interactive visualization (output)
├── .mosaic.json          # Generated state graph (intermediate)
├── collect.py            # List distinct outputs; show a witness path
├── count_bounded.py      # Count walks to terminals up to K steps
├── sample_outputs.py     # Monte Carlo sampling to estimate probabilities
├── sum.py                # Example program under analysis
├── Makefile              # One-command workflows for students
├── requirements.txt      # Python dependencies
└── README.md             # You are here
```

---

## Requirements

* Python 3.10+
* Codespaces or Linux/macOS. Windows WSL is fine.
* The Makefile will set up a local virtual environment in `.venv` and install dependencies from `requirements.txt` (Jinja2, Pygments, rich).

> If your Codespace is a Dev Container, the provided config already has Python. No global install is required.

---

## Quick start (one minute)

### 1) Build the visualization

```bash
make
```

This generates `my_graph.html` and prints next steps.

To serve it in Codespaces:

```bash
make serve
```

Open the Ports panel, right-click the shown port, choose “Open in Browser”. You will see the model checker state transitions.

Stop the server:

```bash
make stop
```

### 2) Generate the full state graph and list possible results

```bash
make list
```

This does two things:

* writes the full graph to `.mosaic.json`
* prints all distinct terminal outputs, for example:

  ```
  Distinct outputs:
    sum = 2    x1
    …
    sum = 9    x1
  ```

The `x1` here means “one terminal node per output after state merging,” not “only one schedule.”

### 3) Ask for a witness path to a specific result

```bash
make witness TARGET='sum = 3'
```

This prints a step-by-step schedule that ends with `sum = 3`.
Edge labels like `main`, `spawn`, `t2`, `t3`, `t4` show which thread stepped next. In this project:

* `t1` is the main thread
* `t2`, `t3`, `t4` are the three spawned worker threads

### 4) Estimate how often each result appears (sampling)

```bash
make sample RUNS=10000
```

This runs random schedules with a step cap and prints estimated probabilities.

### 5) Count how many walks reach each result within K steps

```bash
make count K=200
```

This computes the number of walks that reach each terminal within K steps. Large counts are expected because the graph branches and may have cycles. Use this to compare trends across settings or code variants.

---

## Make targets at a glance

* `make`
  Build `my_graph.html`. Prints guidance for serving it.

* `make serve` / `make stop`
  Start or stop a local HTTP server for the HTML.

* `make list`
  Explore the program, save the graph to `.mosaic.json`, list distinct terminal outputs.

* `make witness TARGET='sum = 3'`
  Print one witness schedule that produces the target output. Uses the prebuilt `.mosaic.json`.

* `make sample RUNS=10000 STEPS=200`
  Monte Carlo sampling on the prebuilt graph. Estimates the relative frequency of each output. Increase `RUNS` or `STEPS` for tighter estimates.

* `make count K=200`
  Count walks up to K steps that reach each terminal output. Works even if the graph has cycles.

* `make clean`
  Remove `my_graph.html`.

* `make clean-graph`
  Remove `.mosaic.json`.

* `make install`
  Create `.venv` and install Python dependencies from `requirements.txt`.

> `witness`, `sample`, and `count` do not re-run the explorer. If `.mosaic.json` is missing they will ask you to run `make list` first.

---

## How the analysis pieces fit together

1. **Exploration (`mosaic.py`)**
   Produces a complete state graph in JSON. Vertices include fields like `stdout`, `choices`, `depth`, and a unique `hashcode`. Edges store source, destination, and a label that shows which thread or action advanced.

2. **Visualization (`vis/__main__.py`)**
   Renders the graph to `my_graph.html` with Jinja2 and Pygments. Students can click through states and see transitions.

3. **Collecting results (`collect.py`)**

   * `--list` prints all distinct terminal outputs. State merging means equal end states share one node, so each output appears as `x1`.
   * `--target` prints a witness path to a terminal whose `stdout` contains the target.

4. **Frequency and counting**

   * `sample_outputs.py` approximates how often each output appears via random walks with a step cap.
   * `count_bounded.py` counts walks up to K steps to each terminal. Values grow fast because branching and cycles create many walks. This is a dynamic program over K layers, which is why it is fast.

---

## Typical student workflow

1. Read `sum.py`. Predict the smallest and largest `sum`.
2. Run `make` and open the HTML to inspect states and transitions.
3. Run `make list` to see all final outputs the program can produce.
4. Use `make witness TARGET='sum = 2'` to see one schedule that leads to the minimum.
5. Optional: run `make sample` to get a sense of which results are more common.

---

## Notes and tips

* Large HTML files are not committed. The `.gitignore` excludes `my_graph.html`. Build locally with `make` and serve it with `make serve`.
* In Codespaces, the server opens in the Ports panel. Right-click the forwarded port and choose “Open in Browser”.
* If you see a warning like `skipped edges due to id mapping`, update the analysis scripts to match your JSON schema. In this repo the scripts map edges by vertex `hashcode`, so you should not see skipped edges when `.mosaic.json` came from `mosaic.py` in this tree.
* If you want “exact path counts” without cycles, you can filter edges to those where `depth` increases by 1 and then set `K` to the max depth. This turns the graph into a DAG for counting. The current `count_bounded.py` does general bounded walks.

---

## Reproducibility

* All analysis targets run with the interpreter in `.venv` so that dependencies are consistent.
* The Makefile prints clear next steps after each command. Students only need `make`, `make serve`, and `make list` to start.

---

## License and acknowledgments

Use this material for teaching and learning. If you extend the explorer or the UI, please keep commands simple so students can run everything with a few `make` targets.
