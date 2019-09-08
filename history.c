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
#include <time.h>
#include <grp.h>
#include <pwd.h>

void history()
{
    int numHist = 0;
    char *historyFile = (char *)malloc(1024 * sizeof(char));
    strcpy(historyFile, shellHome);
    strcat(historyFile, "/");
    strcat(historyFile, ".history.txt");

    //count numHist, i.e, number of lines
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
    int n;

    //to calculate n
    if (Commands[currCommand].argumentsIndex == 0)
        n = 10;
    else
        n = atoi(Commands[currCommand].arguments[0]);

    //to print history
    if (numHist > n)
    {
        int i = 0;
        while (i < (numHist - n))
            fgets(line, sizeof(line), fp), i++;
    }

    while (fgets(line, sizeof(line), fp))
        dprintf(Commands[currCommand].outputFd, "%s", line);

    fclose(fp);
}

void writeToHistory(char *inputCopy)
{
    int numHist = 0;

    char *historyFile = (char *)malloc(1024 * sizeof(char));
    strcpy(historyFile, shellHome);
    strcat(historyFile, "/.history.txt");

    //count numHist, i.e, number of lines
    FILE *fp = fopen(historyFile, "r+");
    char chr = getc(fp);
    while (chr != EOF)
    {
        if (chr == '\n')
            numHist++;
        chr = getc(fp);
    }

    fseek(fp, 0, SEEK_SET);
    char str[1024];
    while (fgets(str, 1024, fp))
        ;
    // strcat(inputCopy, "\n");
    str[strlen(str) - 1] = '\0';
    if (!strcmp(str, inputCopy))
        return;

    if (numHist == 20)
    {
        fseek(fp, 0, SEEK_SET);
        char buffer[20][1024];
        int i = 0;

        while (fgets(str, 1024, fp))
            strcpy(buffer[i++], str);

        fseek(fp, 0, SEEK_SET);

        for (int i = 1; i < 20; i++)
            fprintf(fp, "%s", buffer[i]);
        // printf("old=%s, new=%s\n", historyFile, newHistoryFile);
    }

    fprintf(fp, "%s\n", inputCopy);
    fclose(fp);
}