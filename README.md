# operatingsystems
# Mav Shell (msh)

## Overview
Mav Shell (msh) is a custom shell program written in C, inspired by traditional shells like Bash, Csh, and Ksh. It executes user commands by forking child processes and handling input efficiently.

## Features
- Displays a prompt (`msh>`) and waits for user input.
- Executes any valid system command from `/bin`, `/usr/bin`, `/usr/local/bin`, and the current directory.
- Supports up to **10 command-line arguments**.
- Maintains a **history** of the last **15 commands**.
- Stores and displays the **last 15 process IDs (PIDs)** of executed commands.
- Implements built-in commands:
  - `cd`: Changes directories, supporting `cd ..`
  - `pidhistory`: Lists the last 15 process IDs
  - `history`: Displays command history
  - `!n`: Re-executes the `n`th command from history
  - `exit` or `quit`: Terminates the shell

## Functional Requirements
- Commands are executed using `fork()`, `wait()`, and an `exec` function (e.g., `execvp()`).
- Blank input lines are ignored without any output.
- Proper memory management and error handling.

## Compilation & Execution
Ensure you are using **GCC** on the `omega.uta.edu` server or a compatible Linux environment.

### **Compile:**
```sh
gcc msh.c -o msh

