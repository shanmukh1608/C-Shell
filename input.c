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

char *input;
char *command;
char *flags;
char *arguments;
char *currCommand;

void parseInput()
{
    int i = 0, j = 0;

    command = (char *)malloc(1024 * sizeof(char));
    flags = (char *)malloc(1024 * sizeof(char));
    arguments = (char *)malloc(1024 * sizeof(char));

    while (i < strlen(currCommand) && currCommand[i] == ' ')
        i++;

    while (i < strlen(currCommand) && currCommand[i] != ' ')
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = currCommand[i];
        strcat(command, cToStr);
        i++;
    }

    while (i < strlen(currCommand) && currCommand[i] == ' ')
        i++;

    while (i < strlen(currCommand))
    {
        if (currCommand[i] == '-')
        {
            char cToStr[1024];
            j = 0;

            while (i < strlen(currCommand) && currCommand[i] != ' ')
                cToStr[j++] = currCommand[i++];

            cToStr[j] = ' ';
            cToStr[j + 1] = '\0';
            strcat(flags, cToStr);
        }

        else if (currCommand[i] == ' ')
            i++;

        else
            break;
    }

    int end;
    for (end = strlen(currCommand) - 1; end >= i && currCommand[end] == ' '; end--)
        ;

    while (i <= end)
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = currCommand[i];
        strcat(arguments, cToStr);
        i++;
    }

    // printf("%s\n%s%lld\n%s\n", command, flags, strlen(flags), arguments);
}