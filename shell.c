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
char *currCommand;

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
    while (1)
    {
        int status;
        prompt();
        // printf("%s", shellPrompt);
        printf ("\033[0;1;34m%s\033[0m", shellPrompt);

        input = (char *)malloc(1024 * sizeof(char));
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
        strcpy(inputCopy,input);
        currCommand = strtok(input, ";");

        while (currCommand != NULL)
        {
            parseInput(currCommand);
            writeToHistory(inputCopy); //copy because input has been tokenized

            if (!strcmp(command, "cd"))
                cd();
            else if (!strcmp(command, "echo"))
                echo();
            else if (!strcmp(command, "pwd"))
                pwd();
            else if (!strcmp(command, "pinfo"))
                pinfo();
            else if (!strcmp(command, "ls"))
                ls();
            else if (!strcmp(command, "history"))
                history();
            else if (!strcmp(command, "nightswatch"))
                nightswatch();
            else if (!strcmp(command, "exit"))
                exit(0);
            else
                execInput();

            currCommand = strtok(NULL, ";");
        }
    }
}

int main()
{
    if (getcwd(shellHome, sizeof(shellHome)) != NULL)
        ;
    else
    {
        perror("getcwd() error");
        return 1;
    }

    mainPID = (int)getpid();
    strcpy(processStack[pidTop], "shell");
    pidStack[pidTop] = mainPID;
    pidTop++;
    commandLoop();
    return 0;
}