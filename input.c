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
    char **arr = (char **)malloc(100 * sizeof(char *));
    for (int k = 0; k < 100; k++)
        arr[k] = (char *)malloc(1024);

    while (i < strlen(input))
    {
        while (input[i] != ' ' && i < strlen(input))
        {
            char cToStr[2];
            cToStr[1] = '\0';
            cToStr[0] = input[i];
            strcat(arr[j], cToStr);
            i++;
        }

        i++, j++;
    }

    command = (char *)malloc(1024 * sizeof(char));
    flags = (char *)malloc(1024 * sizeof(char));
    arguments = (char *)malloc(1024 * sizeof(char));

    strcat(command, arr[0]);

    for (i = 1; i <= j && arr[i][0] == '-'; i++)
    {
        strcat(flags, arr[i]);
        strcat(flags, " ");
    }

    for (i; i <= j; i++)
    {
        strcat(arguments, arr[i]);
        strcat(arguments, " ");
    }
}