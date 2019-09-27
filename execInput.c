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
#include "jobs.h"
#include "cronjob.h"

char *buf[1024];

void executeUp()
{
    int count = 0;
    for (int i = 0; i < strlen(input); i++)
        if (input[i] == '\033')
            count++;

    int numHist = 0;
    char *historyFile = (char *)malloc(1024 * sizeof(char));
    strcpy(historyFile, shellHome);
    strcat(historyFile, "/");
    strcat(historyFile, ".history.txt");

    FILE *fp = fopen(historyFile, "r");
    char chr = getc(fp);
    while (chr != EOF)
    {
        if (chr == '\n')
            numHist++;
        chr = getc(fp);
    }

    fseek(fp, 0, SEEK_SET);
    char line[1024];

    if (count > numHist)
    {
        printf("Error: History doesn't have those many commands.\n");
        return;
    }

    else if (numHist > count)
    {
        int i = 0;
        while (i < (numHist - count))
            fgets(line, sizeof(line), fp), i++;
    }

    char command[200];

    fgets(line, sizeof(line), fp);
    sprintf(command, "%s", line);
    strcpy(input, command);
    if (input[strlen(input) - 1] == '\n')
        input[strlen(input) - 1] = '\0';
    prompt();
    printf("\033[0;1;34m%s\033[0m", shellPrompt);
    printf("%s\n", input);

    char inputCopy[1024];
    strcpy(inputCopy, input);
    sepInput = strtok(input, ";");

    while (sepInput != NULL)
    {
        writeToHistory(inputCopy); //copy because input has been tokenized

        if (!strcmp(sepInput, ""))
            continue;
        pipeIndex = 0;
        strcpy(pipeSeparated[0], "");
        separatePipes();

        for (currCommand = 0; currCommand <= pipeIndex; currCommand++)
        {
            currInput = (char *)malloc(1024);
            strcpy(currInput, pipeSeparated[currCommand]);
            if (!parseInput())
                Commands[currCommand].errorFlag = 1;
        }

        for (currCommand = 0; currCommand <= pipeIndex; currCommand++)
        {
            if (pipeIndex == 0)
            {
                dup2(Commands[currCommand].inputFd, STDIN_FILENO);
                dup2(Commands[currCommand].outputFd, STDOUT_FILENO);
                execCommand();
                dup2(savestdin, STDIN_FILENO);
                dup2(savestdout, STDOUT_FILENO);
            }
            else if (currCommand != pipeIndex)
                execPipe();
            else
            {
                dup2(Commands[currCommand].outputFd, 1);
                execCommand();
            }
        }

        dup2(savestdin, STDIN_FILENO);
        dup2(savestdout, STDOUT_FILENO);
        sepInput = strtok(NULL, ";");
    }
}

void procExit()
{
    int status;

    for (int i = 0; i < pidTop; i++)
    {
        if (waitpid(pidStack[i], &status, WNOHANG) > 0)
        {
            deleteJob(pidStack[i]);
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
    else if (!strcmp(Commands[currCommand].command, "jobs"))
        jobs();
    else if (!strcmp(Commands[currCommand].command, "kjob"))
        kjob();
    else if (!strcmp(Commands[currCommand].command, "overkill"))
        overkill();
    else if (!strcmp(Commands[currCommand].command, "fg"))
        fg();
    else if (!strcmp(Commands[currCommand].command, "bg"))
        bg();
    else if (!strcmp(Commands[currCommand].command, "cronjob"))
        cronjob();
    else if (!strcmp(Commands[currCommand].command, "quit"))
        exit(0);
    else
    {
        int status;
        // signal(SIGCHLD, procExit);
        pid_t pid = fork();
        if (pid == 0)
        {
            setpgid(0, 0);

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

            if (!Commands[currCommand].backgroundFlag)
            {
                fgPid = pid;
                waitpid(pid, &status, WUNTRACED);
                if (WIFSTOPPED(status))
                {
                    char stringToAdd[500];
                    strcpy(stringToAdd, Commands[currCommand].command);
                    strcat(stringToAdd, " ");
                    for (int i = 0; i < Commands[currCommand].flagsIndex; i++)
                    {
                        strcat(stringToAdd, Commands[currCommand].flags[i]);
                        strcat(stringToAdd, " ");
                    }

                    for (int i = 0; i < Commands[currCommand].argumentsIndex; i++)
                    {
                        strcat(stringToAdd, Commands[currCommand].arguments[i]);
                        strcat(stringToAdd, " ");
                    }
                    addJob(pid, stringToAdd);
                }
                fgPid = 0;
            }

            else
            {
                char stringToAdd[500];
                strcpy(stringToAdd, Commands[currCommand].command);
                strcat(stringToAdd, " ");
                for (int i = 0; i < Commands[currCommand].flagsIndex; i++)
                {
                    strcat(stringToAdd, Commands[currCommand].flags[i]);
                    strcat(stringToAdd, " ");
                }

                for (int i = 0; i < Commands[currCommand].argumentsIndex; i++)
                {
                    strcat(stringToAdd, Commands[currCommand].arguments[i]);
                    strcat(stringToAdd, " ");
                }

                addJob(pid, stringToAdd);
            }
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