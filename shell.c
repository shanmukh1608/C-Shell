#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "input.h"
#include "globals.h"
#include "pwd.h"
#include "cd.h"
#include "echo.h"
#include "ls.h"
#include "pinfo.h"
#include "execInput.h"
#include "history.h"
#include "nightswatch.h"

void basicHandler(int sig)
{
    printf("\n");
    if (fgPid)
        kill(fgPid, sig);
}

char state;
int vm_result;
char shellPWD[1024];
char shellHome[1024];
char mon[4];
int mainPID;
int pidTop;
int pidStack[1024];
char processStack[1024][1024];
char *shellPrompt;
char *input;
char *sepInput;
int currCommand;
int savestdin;
int savestdout;

struct commandStruct Commands[1024];

void prompt()
{
    shellPrompt = (char *)malloc(1024);

    //To get user name
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    strcpy(shellPrompt, "<");
    strcat(shellPrompt, hostname);
    strcat(shellPrompt, "@");

    //To get system name
    struct utsname unameData;
    uname(&unameData);
    strcat(shellPrompt, unameData.sysname);
    strcat(shellPrompt, ":");

    //To check if PWD is at home, at subdirectory of home, or superdirectory of home
    if (getcwd(shellPWD, sizeof(shellPWD)) != NULL)
    {
        if (!strcmp(shellPWD, shellHome)) //at home directory
            strcpy(shellPWD, "~");

        else if (strstr(shellPWD, shellHome)) //at subdirectory of home
        {
            char *temp = malloc(strlen(shellPWD));
            strcpy(temp, shellPWD);
            strcpy(shellPWD, "~");
            int homeLength = strlen(shellHome);
            // printf("Shellhome=%s, homelength=%d, temp=%s, templength=%d", shellHome, homeLength, temp, strlen(temp));
            for (int i = homeLength; i < strlen(temp); i++)
                shellPWD[i - homeLength + 1] = temp[i];
            shellPWD[strlen(temp) - homeLength + 1] = '\0';
            free(temp);
        }
    }

    strcat(shellPrompt, shellPWD);
    strcat(shellPrompt, ">");
}

void commandLoop()
{
    savestdin = dup(0);
    savestdout = dup(1);
    while (1)
    {
        int status;
        prompt();

        printf("\033[0;1;34m%s\033[0m", shellPrompt);

        input = (char *)malloc(1024 * sizeof(char));
        strcpy(input, "");
        while (1)
        {
            char ch;
            scanf("%c", &ch);
            if (ch == '\n')
                break;

            char cToStr[2];
            cToStr[1] = '\0';
            cToStr[0] = ch;
            strcat(input, cToStr);
        }

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
}

int main()
{
    signal(SIGINT, basicHandler);
    signal(SIGQUIT, basicHandler);
    signal(SIGTSTP, basicHandler);
    fgPid=0;

    if (getcwd(shellHome, sizeof(shellHome)) != NULL)
        ;
    else
    {
        perror("getcwd() error");
        return 1;
    }

    mainPID = (int)getpid();
    strcpy(processStack[pidTop], "shell");
    pidStack[pidTop++] = mainPID;
    commandLoop();
    return 0;
}