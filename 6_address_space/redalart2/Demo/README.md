The operating system provides mechanisms that allow us to implement debuggers, enabling us to inspect and even modify the code and data of any process. This capability allows us to bypass the "artificial barriers" set by the game, gaining more money, experience, or even locking health points.

# Game Memory Modifier

This tool is a simple C++ program that demonstrates how to search for and modify the memory data of a running process. It is intended as an educational example for building a game cheat engine. **Note:** This program must be run on a Linux system with root permissions because it directly accesses another process's memory via `/proc/<pid>/mem`.

## Overview

The program operates by:
- Retrieving the target process's PID using the `pidof` command.
- Opening the target process's memory file (`/proc/<pid>/mem`) with read-write permissions.
- Scanning the target process's memory for a specified 32-bit value by reading memory regions obtained from `pmap -x`.
- Filtering and storing the addresses where the target value is found.
- Allowing the user to overwrite those memory addresses with a new value.

## How It Works

1. **Initialization:**
   - The `Game` struct is instantiated with the name of the target process.
   - The constructor runs the command `pidof <process_name>` to obtain the process ID.
   - It then constructs the path `/proc/<pid>/mem` and opens this file for read-write access. This access requires root privileges.

2. **Memory Scanning:**
   - The `search_for(uint32_t val)` function is used to search for a specific 32-bit value in the target process's memory.
   - **First Search:**
     - If the `remain` vector is empty (i.e., no previous search results), the program runs `pmap -x <pid>` to get the memory map of the process.
     - A regular expression is used to parse the memory map and identify regions with read-write permissions.
     - The program then reads each memory region into a local buffer and scans for the target value. Matching addresses are stored in the `remain` vector.
   - **Subsequent Searches:**
     - The program re-scans the addresses stored in `remain` to check if they still contain the target value. Addresses that no longer match are removed.

3. **Memory Modification:**
   - The `overwrite(uint32_t val)` function iterates over the addresses stored in `remain` and writes a new 32-bit value to each location, thereby modifying the target process's memory.

4. **User Interaction:**
   - The program uses a simple command-line interface that supports:
     - **`s <value>`**: Search for a specified value in memory.
     - **`w <value>`**: Overwrite the found values with a new value.
     - **`r`**: Reset the search (clear previously found addresses).
     - **`q`**: Quit the program.

## Key Functions

- **`Game::Game(string proc_name)`**  
  Initializes the `Game` object by obtaining the target process's PID and opening its memory file.

- **`Game::search_for(uint32_t val)`**  
  Searches for the specified 32-bit value in the target process's memory. It performs a full memory scan on the first search and incremental filtering on subsequent searches.

- **`Game::overwrite(uint32_t val)`**  
  Overwrites each of the addresses stored in the `remain` vector with the new specified value.

- **`Game::reset()`**  
  Clears the vector containing the matched memory addresses, resetting the search.

- **`Game::load(uintptr_t addr)`** and **`Game::store(uintptr_t addr, uint32_t val)`**  
  Helper functions to read a 32-bit value from and write a 32-bit value to a specified memory address in the target process.

- **`Game::run(const string &cmd)`**  
  Executes a shell command and returns its output as a string. This function is used to run `pidof` and `pmap -x`.

## Usage

1. **Compilation:**  
   Compile the program using a C++ compiler (with C++17 support), for example:
   ```bash
   g++ -std=c++17 -o memory_modifier memory_modifier.cpp
