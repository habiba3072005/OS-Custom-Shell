# myShell

A simple command-line shell written in C, supporting built-in commands, I/O redirection, pipes, background execution, and signal handling.

##  How to Compile and Run

```bash
make
./myShell
```

##  Features

- Custom prompt: `myShell>`
- Reads and parses user input
- Executes commands using `fork()` and `execvp()`
- Exit the shell with `exit`
- Background execution using `&`
- Built-in commands: `cd`, `pwd`, `history`, `exit`
- Signal handling for `Ctrl+C` and `Ctrl+Z`
- Input/output redirection using `<` and `>`
- Pipes support using `|`
- Error handling via `perror()` and `errno`
- Validation for system calls: `fork`, `execvp`, `pipe`, `open`
- Directory validation for `cd` using `access()`
- Makefile for easy compilation

##  Technical Implementation Details

- **Zombie process cleanup:** a `SIGCHLD` handler with `waitpid(..., WNOHANG)` reaps background processes so they don't hang around.
- **Command history:** stored in a circular buffer (size 50) to avoid memory overflow.
- **Signal handling:** `Ctrl+C` and `Ctrl+Z` are ignored by the shell itself but still work normally for child processes.
- **Pipeline logic:** supports chaining multiple commands by properly managing file descriptors.

##  Reliability & Robustness

- Checks `HOME` before using it in `cd`
- Uses `access()` to verify a directory exists before switching to it
- Every system call is checked, with errors printed via `perror()`
- If redirection fails, only the child process stops — not the whole shell

##  System Constraints

| Constraint | Limit |
|---|---|
| Max input size | 1024 characters |
| Max arguments per command | 64 |
| History size | 50 commands |
| Max path length | 1200 characters |

##  Examples

### Basic Commands
```
myShell> ls
myShell> ls -l
myShell> pwd
myShell> cd /home
myShell> sleep 5 &
myShell> history
myShell> exit
```

### Redirection
```
myShell> ls -l > output.txt
myShell> sort < input.txt
myShell> cat < input.txt > output.txt
```

### Pipes
```
myShell> ls -l | grep .c
myShell> ps aux | grep myShell
```

### Error Handling
```
myShell> cd /non_existent_folder
--> cd failed: No such file or directory

myShell> ls | non_existent_command
--> Error: Command not found

myShell> cat < missing.txt
--> open: No such file or directory
```

##  Project Structure

```
myShell/
├── myShell.c
├── Makefile
└── README.md
```

##  Team Contributions

**Team 1** — Main loop, parser, execution
- Mahmoud Ezzat Morsi — 23011518
- Zeyad Yasser Mahros — 23011079
- Mohamed Ahmed Ramadan — 23011452

**Team 2** — Built-ins, background execution, signals
- Amar Mahmoud Mohamed — 23011114
- Abdelrahman Mostafa Mohamed — 23011350
- Abdelhamid Anwar Abdelhamid — 23011099

**Team 3** — Redirection, pipes
- Habiba Hamdy Ali — 23011064
- Habiba Samy Mohamed — 23010137

**Team 4** — Error handling, Makefile, README
- Mariam Mohamed Sadik — 23011153
- Habiba Yahya Shabaan — 23011066
