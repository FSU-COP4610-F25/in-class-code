**SIMT Model**: In this non-executable model, we mimic an NVIDIA-style SIMT processor. The key feature is that there is no per-thread program counter. A warp of threads shares one program counter and executes in lockstep. You can think of the threads in a warp as a “programmable” form of SIMD.

