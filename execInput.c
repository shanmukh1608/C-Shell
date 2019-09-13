#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include "globals.h"
#include "ls.h"

int count = 1;
char proc_stack[1024][1024];
char *buf[1024];

void procExit()
{
    int status;

    for (int i = 1; i < pidTop; i++)
    {
        if (waitpid(pidStack[i], &status, WNOHANG) > 0)
        {
            if (WIFEXITED(status) > 0)
                printf("%s with pid %d exited normally\n", processStack[i], pidStack[i]);
            else if (WIFSIGNALED(status))
                printf("%s with pid %d exited with signal\n", processStack[i], pidStack[i]);
            else
                printf("%s with pid %d exited abnormally\n", processStack[i], pidStack[i]);
        }
    }
}

int createBuf()
{
    buf[0] = (char *)malloc(1024);
    strcpy(buf[0], Commands[currCommand].command);

    int bufSize = 1;

    char *temp;
    temp = (char *)malloc(1024);

    for (int i = 0; i < Commands[currCommand].flagsIndex; i++)
    {
        buf[bufSize] = (char *)malloc(1024);
        strcpy(buf[bufSize++], Commands[currCommand].flags[i]);
    }

    for (int i = 0; i < Commands[currCommand].argumentsIndex; i++)
    {
        buf[bufSize] = (char *)malloc(1024);
        strcpy(buf[bufSize++], Commands[currCommand].arguments[i]);
    }

    buf[bufSize] = NULL;

    return buf;
}

void execCommand()
{
    if (!strcmp(Commands[currCommand].command, "cd"))
        cd();
    else if (!strcmp(Commands[currCommand].command, "echo"))
        echo();
    else if (!strcmp(Commands[currCommand].command, "pwd"))
        pwd();
    else if (!strcmp(Commands[currCommand].command, "pinfo"))
        pinfo();
    else if (!strcmp(Commands[currCommand].command, "ls"))
        ls();
    else if (!strcmp(Commands[currCommand].command, "history"))
        history();
    else if (!strcmp(Commands[currCommand].command, "nightswatch"))
        nightswatch();
    else if (!strcmp(Commands[currCommand].command, "setenv"))
        setEnv();
    else if (!strcmp(Commands[currCommand].command, "unsetenv"))
        unsetEnv();
    else if (!strcmp(Commands[currCommand].command, "exit"))
        exit(0);
    else
    {
        int status;
        signal(SIGCHLD, procExit);
        pid_t pid = fork();
        if (pid == 0)
        {
            createBuf();
            if (currCommand == 0)
                dup2(Commands[currCommand].inputFd, 0);
            if (execvp(buf[0], buf) < 0)
            {
                printf("Error executing\n");
                exit(1);
            }
        }

        else if (pid > 0)
        {
            pidStack[pidTop] = pid;
            strcpy(processStack[pidTop++], Commands[currCommand].command);

            if (Commands[currCommand].backgroundFlag)
                printf("[%d] %d\n", count++, pid);
            else
                waitpid(pid, &status, 0);
        }

        else
            printf("fork error\n");

        return status;
    }
}

void execPipe()
{
    int fd[2];
    pipe(fd);
    // printf("%s\n", Commands[currCommand].command);
    pid_t pid = fork();

    if (pid == 0)
    {
        dup2(fd[1], 1);
        execCommand();
        abort();
    }

    else if (pid > 0)
    {
        dup2(fd[0], 0);
        close(fd[1]);
    }

    else
        printf("fork error\n");

    close(fd[0]);
    close(fd[1]);
}
