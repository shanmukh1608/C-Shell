# C-Shell

Run "make all" to create shell, run "./shell" to execute

cd.c - cd command
cronjob.c - cronjob command
echo.c - echo command
execInput.c - executing commands
globals.c - contains functions that are used across various files
history.c - history command, also contains function to write to history after each command is executed
input.c - parse input
jobs.c - contains all commands in specification 5 of Assignment 3 (jobs, kjob, fg, bg, overkill, etc)
ls.c - ls command
nightswatch.c - nightswatch command
pinfo.c - pinfo command
pwd.c - pwd command
setenv.c - setenv and unsetenv command
shell.c - main file, responsible for printing prompt, taking input, and calling respective functions