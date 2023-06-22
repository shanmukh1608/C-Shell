# C-Shell

## Introduction

This is an implementation of a Linux shell written in C language with support for piping, I/O redirection, background and foreground processes,
history retrieval, custom commands, etc.

## Run the shell

Run "make all" to create shell, run "./shell" to execute

## Functionality and Limitations
- Built-in commands: `ls [al]`, `echo`, `pwd`, `cd`, `quit`, `history`, `pinfo`, `nightswatch`, `setenv`, `unsetenv`, `jobs`, `kjobs`, `overkill`, `fg`, `bg`, `cronjob`, `quit`
- Process management including foreground and background processes and switching between them.
- Piping and redirection
- Signal handling for signals like `SIGINT` and `SIGTSTP`
- A prompt that shows the username, hostname and the current working directory
- Command history and <kbd>up</kbd> arrow command recall across sessions
- Appropriate error handling

## File Breakdown

- cd.c - cd command
- echo.c - echo command
- execInput.c - external commands
- globals.c - contains functions that are used across various files
- history.c - history command, also contains function to write to history after each command is executed
- input.c - parse input
- ls.c - ls command
- nightswatch.c - nightswatch command
- pinfo.c - pinfo command
- pwd.c - pwd command
- shell.c - main file, responsible for printing prompt, taking input, and calling respective functions
