# Author: o3-mini
# (This code is not well written)

# Set a breakpoint at main and start the program
break main
run

python
import gdb
import json
import re

def get_stack_state():
    """
    Return a list of frames from main (bottom of the call stack) to the newest frame.
    Each item is a dict:
      {"function": <function_name>, "locals": {<var>: <value>, ...}}
    Note: only collects locals that GDB can read. Arguments are not included.
    """
    state = []
    # Walk from the newest frame upward
    frame = gdb.newest_frame()
    while frame is not None:
        try:
            fname = frame.name() or "??"
            if '__' in fname:
                break
            
            local_vars = {}
            # Iterate symbols via frame.block (may include inner-block variables)
            block = frame.block()
            while block:
                for symbol in block:
                    if symbol.is_variable or symbol.is_argument:
                        try:
                            # Try to read the variable value
                            v = symbol.value(frame)
                            if hasattr(v, 'address'):
                                if v.address > 0x6fffffffffff:
                                    local_vars[symbol.print_name] = str(v)
                                elif not state:
                                    local_vars[symbol.print_name] = str(v)
                        except Exception:
                            pass
                block = block.superblock
            state.append({"function": fname, "locals": local_vars})
        except Exception:
            pass
        frame = frame.older()
    state.reverse()
    # Keep frames starting from main only (assume main is the entry)
    for i, frame_info in enumerate(state):
        if frame_info["function"] == "main":
            return state[i:]
    return state

def format_state(state):
    """
    Format the call stack state into a single line of text.
    Each frame is rendered as its locals, one per line: var = val
    Frames are separated with an <hr>.
    """
    lines = []
    for frame in state:
        # Join locals as HTML lines: "var = val"
        locals_str = "<br>".join(f"{k} = {v}" for k,v in frame["locals"].items())
        lines.append(locals_str)
    return "<hr>".join(lines)

# Store per-step transition records as tuples: (stack_state, label)
transitions = []

def current_line():
    frame = gdb.newest_frame()
    sal = frame.find_sal()
    with open(sal.symtab.filename, 'r') as f:
         lines = f.readlines()
         line = sal.line
         return f"{line} - " + lines[line - 1].replace(";", "").strip()

# Record the state when we stop at the entry of main
init_state = get_stack_state()
transitions.append((init_state, current_line()))

# Start single-step tracing
while True:
    prev_state = transitions[-1][0]
    prev_depth = len(prev_state)
    try:
        # Execute one line. 'step' enters functions.
        gdb.execute("step", to_string=True)
    except gdb.error:
        # If an error occurs (for example the program exits), stop the loop
        break

    # Try to obtain the current source line using the current frame's symtab info
    try:
        code_line = current_line()
    except Exception:
        code_line = "unknown"

    new_state = get_stack_state()
    new_depth = len(new_state)

    # Detect calls or returns by comparing stack depth
    if new_depth > prev_depth:
        label = "call " + new_state[-1]["function"]
    elif new_depth < prev_depth:
        label = code_line
    else:
        label = code_line

    transitions.append( (new_state, label) )

    # End tracing if main is no longer in the call stack (main returned)
    if not any(frame["function"] == "main" for frame in new_state):
        break

# After debugging ends, write plot.md in Mermaid stateDiagram-v2 format
try:
    with open("plot.md", "w", encoding="utf-8") as f:
        f.write("```mermaid\n")
        f.write("stateDiagram-v2\n")

        for i, (state, _) in enumerate(transitions):
            f.write(f"    S{i}: {format_state(state)}\n")

        # Define state nodes using indices S0, S1, ...
        # The state content is the formatted locals per frame
        def state_name(i, state):
            return "S{}".format(i)
        # Initial state
        f.write("    [*] --> S0\n")
        for i in range(1, len(transitions)):
            f.write("    " + state_name(i-1, transitions[i-1][0]) + " --> " + state_name(i, transitions[i][0]) +
                    " : " + transitions[i-1][1] + "\n")
        f.write("```\n")
    print("plot.md generated.")
except Exception as e:
    print("Error writing plot.md:", e)

end

# Exit gdb after the program finishes
quit
