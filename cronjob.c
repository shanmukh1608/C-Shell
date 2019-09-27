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
#include "globals.h"
#include "execInput.h"

void cronjob()
{
    char cronFlags[500];
    strcpy(cronFlags, Commands[currCommand].flags[0]);
    for (int i = 0; i < Commands[currCommand].argumentsIndex; i++)
    {
        strcat(cronFlags, " ");
        strcat(cronFlags, Commands[currCommand].arguments[i]);
    }

    char command[200], periodString[200], tillString[200];
    strcpy(command, "\0");
    int period, till;

    int i = 0;
    while (i < strlen(cronFlags) && !(cronFlags[i] == '-' && cronFlags[i + 1] == 'c'))
        i++;
    i += 2;

    while (i < strlen(cronFlags) && !(cronFlags[i] == '-' && cronFlags[i + 1] == 't'))
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = cronFlags[i++];
        strcat(command, cToStr);
    }
    i += 2;

    while (i < strlen(cronFlags) && !(cronFlags[i] == '-' && cronFlags[i + 1] == 'p'))
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = cronFlags[i++];
        strcat(periodString, cToStr);
    }
    i += 2;

    while (i < strlen(cronFlags))
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = cronFlags[i++];
        strcat(tillString, cToStr);
    }

    period = atoi(periodString);
    till = atoi(tillString);

    // printf("command=%s\n", command);
    int pid = fork();

    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        int cycles = till / period;
        while (cycles--)
        {
            sleep(period);

            strcpy(currInput, command);
            parseInput();
            if (!strcmp(Commands[currCommand].command, "cd") || !strcmp(Commands[currCommand].command, "echo") || !strcmp(Commands[currCommand].command, "pwd") ||
                !strcmp(Commands[currCommand].command, "pinfo") || !strcmp(Commands[currCommand].command, "ls") || !strcmp(Commands[currCommand].command, "history") ||
                !strcmp(Commands[currCommand].command, "nightswatch") || !strcmp(Commands[currCommand].command, "setenv") || !strcmp(Commands[currCommand].command, "unsetenv") ||
                !strcmp(Commands[currCommand].command, "jobs") || !strcmp(Commands[currCommand].command, "kjob") || !strcmp(Commands[currCommand].command, "overkill") ||
                !strcmp(Commands[currCommand].command, "fg") || !strcmp(Commands[currCommand].command, "bg") || !strcmp(Commands[currCommand].command, "cronjob"))
                ;
            else
                Commands[currCommand].backgroundFlag = 1;

            execCommand();
        }
        exit(0);
    }
    else if (pid<0)
    {
        printf("Fork error\n");
    }
}