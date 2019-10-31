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

int backgroundFlag;
char *input;
char *command;
char *flags;
char *arguments;
char *currCommand;

void parseInput()
{
    size_t i = 0;
    int j = 0;

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

    size_t curr = i;
    while (i < strlen(currCommand))
    {
        if (i == curr)
            if (isDigit(currCommand[i]))
                break;

        if (currCommand[i] == '-' || isDigit(currCommand[i]))
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

    flags[strlen(flags) - 1] = '\0';
    backgroundFlag = 0;

    size_t end;
    for (end = strlen(currCommand) - 1; end >= i && (currCommand[end] == ' ' || currCommand[end] == '&'); end--)
        if (currCommand[end] == '&')
            backgroundFlag = 1;

    while (i <= end)
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = currCommand[i];
        strcat(arguments, cToStr);
        i++;
    }

    // printf("%s\n%s\n%s\n%d\n", command, flags, arguments, backgroundFlag);
}
