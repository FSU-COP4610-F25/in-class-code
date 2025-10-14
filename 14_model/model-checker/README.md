# Model Checker

## Overview

Model checking is a method for formally verifying finite-state systems.  
As long as a system model can be established, it can be **proven correct** or **errors can be found** using brute-force verification.

## Key Concept: State Machine

Everything in a **model checker** revolves around **state machines**!

### **Safety**  

- **Red states must not be reachable**  
- This is a **reachability problem** in the graph **G(V, E)**

### **(Strong) Liveness**  

- **From any state, it must always be possible to reach a green/blue state**  
- What kind of problem does this represent in **G(V, E)**?

### Useage

We recommend using a virtual environment (optional but encouraged):
'''
python3 -m venv venv
source venv/bin/activate
'''

Install required dependencies:
‘’‘
brew install graphviz  # （macOS only, required for rendering graphs) 
pip install graphviz jinja2 markdown astor

’‘’

Using make:

'''
make FILE=mutex-bad.py
'''

Or manually:

'''
python3.11 model-checker.py mutex-bad.py | python3.11 visualize.py > display.html
'''

Or for simplicity:

'''
python3.11 model-checker.py mutex-bad.py | python3.11 visualize.py -t > display.html
'''

### **Key Questions**

- How can we **visualize** this state machine?  
- How can we **avoid inefficient exploration**?

