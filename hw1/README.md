CS5374 Homework 1 
===
* Name: 楊卓敏
* Student ID: r12944068

### File Structure

    ├── include
    │   └── shell.h
    ├── Makefile
    ├── README.md
    └── src
        ├── main.c
        └── shell.c

### How to execute 
1. generate the binary cs5374_sh
    * `make`
2. copy the binary cs5374_sh to `./test/` directory
    * `cp cs5374_sh ./test/`

### Code Implementation
1. The main function read a line of command using `getline` function and store the command into a command buffer. Also, register a signal handler to free allocated memory for `SIGINT` signal.
2. Since it is required to parse the command to arguments, in order not to affect the string stored in the command buffer, I store a copy of the current command and parse the copied command.
3. Check the current command is a pipe command or not, if not, run the `execShell` function directly
4. If it is a pipe command, parse the pipe command into multiple sub-commands and determine the number of `pipefd` should be created. After parsing, create `N (# of sub-commands)` child processes and connect their input and output file descriptor to the pipe using `dup2` function. The parent process will wait until all the child process has exited.
5. The `execShell` function checks the current command is a built-in command (`cd`, `exit`, `history`) or a binary executable, and outputs the corresponding error message.