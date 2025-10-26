import subprocess
import time
import numpy as np
import matplotlib.pyplot as plt

# Executable names
executables = ['atomic', 'mutex', 'spin']
# Thread counts (arguments)
args = [1, 2, 4, 8, 16]
# Results dict: exec -> arg -> list of run times
results = {exe: {arg: [] for arg in args} for exe in executables}

# Run each executable and record wall time
for exe in executables:
    for arg in args:
        print(f'Run: {exe}, {arg} threads')
        times = []
        for r in range(5):  # 5 trials per setting
            start = time.time()
            o = subprocess.check_output([f"./{exe}", str(arg)])
            end = time.time()
            times.append(end - start)
            print(f'#{r+1} {o.decode().strip()}')
        results[exe][arg] = times
        print()

# Compute median and std for error bars
medians = {exe: [] for exe in executables}
errors = {exe: [] for exe in executables}
for exe in executables:
    for arg in args:
        medians[exe].append(np.median(results[exe][arg]))
        errors[exe].append(np.std(results[exe][arg]))

# Plot
plt.figure(figsize=(10, 6))
for exe in executables:
    plt.errorbar(args, medians[exe], yerr=errors[exe], capsize=5, label=exe)

plt.xlabel('Number of Threads')
plt.ylabel('Time (s)')
plt.title('Execution Time Comparison')
plt.xticks(args)
plt.legend()
plt.grid(True)

# >>> Add this line to save the figure as PNG in the current directory
plt.savefig('execution_time.png', dpi=200, bbox_inches='tight')

plt.show()
