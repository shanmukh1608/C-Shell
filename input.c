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

void Input()
{
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

    int i = 0, j = 0;

    command = (char *)malloc(1024 * sizeof(char));
    flags = (char *)malloc(1024 * sizeof(char));
    arguments = (char *)malloc(1024 * sizeof(char));

    while (i < strlen(input) && input[i] == ' ')
        i++;

    while (i < strlen(input) && input[i] != ' ')
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = input[i];
        strcat(command, cToStr);
        i++;
    }

    while (i < strlen(input) && input[i] == ' ')
        i++;

    while (i < strlen(input))
    {
        if (input[i] == '-')
        {
            char cToStr[1024];
            j = 0;

            while (i < strlen(input) && input[i] != ' ')
                cToStr[j++] = input[i++];

            cToStr[j] = ' ';
            cToStr[j + 1] = '\0';
            strcat(flags, cToStr);
        }

        else if (input[i] == ' ')
            i++;

        else
            break;
    }

    int end;
    for (end = strlen(input) - 1; end >= i && input[end] == ' '; end--)
        ;

    while (i <= end)
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = input[i];
        strcat(arguments, cToStr);
        i++;
    }
}