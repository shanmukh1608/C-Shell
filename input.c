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

char *currInput;

void parseInput()
{
    currCommand = 0;
    Commands[currCommand].flagsIndex = 0;
    Commands[currCommand].argumentsIndex = 0;
    Commands[currCommand].backgroundFlag = 0;
    


    int i = 0, j = 0;

    Commands[currCommand].command = (char *)malloc(1024);

    while (i < strlen(currInput) && currInput[i] == ' ')
        i++;

    while (i < strlen(currInput) && currInput[i] != ' ')
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = currInput[i];
        strcat(Commands[currCommand].command, cToStr);
        i++;
    }

    while (i < strlen(currInput) && currInput[i] == ' ')
        i++;

    int flagStart = i;

    while (i < strlen(currInput))
    {
        if (i == flagStart)
            if (isDigit(currInput[i]))
                break;

        if (currInput[i] == '-' || isDigit(currInput[i]))
        {
            char cToStr[1024];
            j = 0;

            while (i < strlen(currInput) && currInput[i] != ' ')
                cToStr[j++] = currInput[i++];

            cToStr[j] = '\0';
            Commands[currCommand].flags[Commands[currCommand].flagsIndex] = (char *)malloc(1024);
            strcpy(Commands[currCommand].flags[Commands[currCommand].flagsIndex++], cToStr);
        }

        else if (currInput[i] == ' ')
            i++;

        else
            break;
    }

    while (i < strlen(currInput) && currInput[i] != '<' && currInput[i] != '>')
    {
        if (currInput[i] != ' ')
        {
            char cToStr[1024];
            j = 0;
            while (i <= strlen(currInput) && currInput[i] != ' ' && currInput[i] != '<' && currInput[i] != '>')
                cToStr[j++] = currInput[i++];

            cToStr[j] = '\0';
            Commands[currCommand].arguments[Commands[currCommand].argumentsIndex] = (char *)malloc(1024);
            strcpy(Commands[currCommand].arguments[Commands[currCommand].argumentsIndex++], cToStr);
        }

        else
            i++;
    }

    if (Commands[currCommand].argumentsIndex > 0 && !strcmp(Commands[currCommand].arguments[Commands[currCommand].argumentsIndex - 1], "&"))
    {
        Commands[currCommand].backgroundFlag = 1;
        Commands[currCommand].argumentsIndex--;
    }

    while (i != strlen(currInput) && currInput[i] == ' ')
        i++;

    if (i != strlen(currInput) && currInput[i] == '<')
    {
        i++;
        while (i != strlen(currInput) && currInput[i] == ' ')
            i++;

        char cToStr[1024];
        j = 0;
        while (i != strlen(currInput) && currInput[i] != ' ' && currInput[i] != '>')
            cToStr[j++] = currInput[i++];

        cToStr[j] = '\0';
        Commands[currCommand].inputFile = (char *)malloc(1024);
        strcpy(Commands[currCommand].inputFile, cToStr);
    }

    while (i != strlen(currInput) && currInput[i] == ' ')
        i++;

    if (i != strlen(currInput) && currInput[i] == '>')
    {
        i++;
        while (i != strlen(currInput) && currInput[i] == ' ')
            i++;

        char cToStr[1024];
        j = 0;
        while (i != strlen(currInput) && currInput[i] != ' ' && currInput[i] != '>')
            cToStr[j++] = currInput[i++];

        cToStr[j] = '\0';
        Commands[currCommand].outputFile = (char *)malloc(1024);
        strcpy(Commands[currCommand].outputFile, cToStr);
    }
}