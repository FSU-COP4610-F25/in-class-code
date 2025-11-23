# COP4610 In-Class Code (FSU)

<p align="center">
  <a href="https://special-space-fishstick-6944wp4g4wwhrr94.github.dev/">
    <img
      src="https://img.shields.io/badge/CLICK%20THIS%20TO%20OPEN-COP4610%20GitHub%20Codespace-0078D4?style=for-the-badge&logo=github&logoColor=white"
      alt="CLICK THIS TO OPEN COP4610 GITHUB CODESPACE"
      width="1600"
      height="160"
    >
  </a>
</p>


This repository provides lecture code and small runnable examples for **FSU COP4610: Operating Systems**. It is used in class to demonstrate core OS concepts with short, focused programs in C and Linux.


## Purpose
- Support live demos during lectures.
- Offer minimal, readable examples that students can extend in homework and projects.
- Keep all code self-contained and easy to build with `make` and `gcc`.

## Contents
- **processes and threads**: fork, exec, scheduling, synchronization
- **memory**: mmap, paging ideas, sanitizer demos
- **files and IO**: syscalls, buffering, filesystem mini-labs
- **concurrency**: locks, races, deadlocks, debugging helpers
- **drivers (intro)**: toy character devices and userspace stubs

## How to use
1. Clone the repo.
2. Enter a folder and run `make` to build the demo.
3. Read the source and comments, then modify and re-run.
4. Some demos require Linux and a recent `gcc`. A few kernel-module examples must be built and loaded on a VM that matches the running kernel.

## Notes for students
- Code here is for **learning**. It favors clarity over completeness.
- When you reuse or extend examples, please keep attributions.
- Follow your assignment handouts for exact build and run instructions.

## Acknowledgments
Many examples and ideas in this repository are **inspired by and adapted from Prof. Yanyan Jiang’s open Operating Systems course at Nanjing University**. We thank Prof. Jiang for the excellent open course materials and example code that have benefited our teaching and our students.

- Nanjing University OS course by Prof. Yanyan Jiang
- Public course materials and demo code referenced for educational use

## License
Educational use for FSU COP4610. Files adapted from external sources keep their original notices. Please retain authorship and source acknowledgments.
