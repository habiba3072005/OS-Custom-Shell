# myShell
Simple command-line shell written in C
=============================================================
# How to Compile and Run
make
./myShell
=============================================================
# Features
Prompt display: myShell>
Read and parse user input
Execute basic commands using fork() and execvp()
Exit shell using: exit
Support background execution using &
Built-in commands: cd, pwd, history, exit
Signal handling for Ctrl+C and Ctrl+Z
Input/Output redirection using < and >
Pipes support using |
Error handling using perror() and errno
Validation for system calls like fork, execvp, pipe, open
Directory validation for cd using access()
Makefile for compiling the project
=============================================================
# Technical Implementation Details
We handled zombie processes using a SIGCHLD handler with waitpid(..., WNOHANG) so background processes don’t stay hanging.
Command history is stored using a circular buffer (size 50) so we don’t overflow memory.
Ctrl+C and Ctrl+Z are ignored by the shell itself but still work for child processes.
Pipeline logic supports chaining multiple commands by properly managing file descriptors
=============================================================
# Reliability & Robustness
We tried to make the shell stable even with wrong input:
Checked HOME before using it in cd
Used access() to make sure directories exist before switching
Every system call is checked and errors are printed properly
If redirection fails, only the child process stops, not the whole shell
=============================================================
# System Constraints
Max input size: 1024 characters
Max arguments: 64 per command
History size: 50 commands
Max path length: 1200 characters
=============================================================
# Examples
myShell> ls
myShell> ls -l
myShell> pwd
myShell> cd /home
myShell> sleep 5 &
myShell> history
myShell> exit
# Redirection
myShell> ls -l > output.txt
myShell> sort < input.txt
myShell> cat < input.txt > output.txt
# Pipes
myShell> ls -l | grep .c
myShell> ps aux | grep myShell
# Error Handling
myShell> cd /non_existent_folder
--> cd failed: No such file or directory

myShell> ls | non_existent_command
--> Error: Command not found

myShell> cat < missing.txt
--> open: No such file or directory
====================================================
# Project Structure
myShell/
├── myShell.c
├── Makefile
└── README.md
====================================================
# Team Contributions
## Team 1 (Main loop + parser + execution)
Mahmoud Ezzat Morsi — 23011518
Zeyad Yasser Mahros — 23011079
Mohamed Ahmed Ramadan — 23011452
## Team 2 (Built-ins + background + signals)
Amar Mahmoud Mohamed — 23011114
Abdelrahman Mostafa Mohamed — 23011350
Abdelhamid Anwar Abdelhamid — 23011099
## Team 3 (Redirection + pipes)
Habiba Hamdy Ali — 23011064
Habiba Samy Mohamed — 23010137
## Team 4 (Error handling + Makefile + README)
Mariam Mohamed Sadik — 23011153
Habiba Yahya Shabaan — 23011066