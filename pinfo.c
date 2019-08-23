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

char state;
int vm_result;
void proc_info()
{
    char *filelink;
    filelink = (char *)malloc(1024);
    strcat(filelink, "/proc/");
    char pid_string[1024];
    //printf("%s\n", parsed[current_command].arguments[0]);

    if (strcmp(arguments, ""))
        strcat(filelink, arguments);
    else
    {
        char pid_string[1024];
        itoa(mainPID, pid_string, 10);
        strcat(filelink, pid_string);
    }

    strcat(filelink, "/stat");
    FILE *file = fopen(filelink, "r");
    char line[1024];

    fgets(line, 1024, file);
    char *info;
    info = (char *)malloc(1024);
    info = strtok(line, " ");
    int count = 1;
    while (count < 3 && info != NULL)
    {
        info = strtok(NULL, " ");
        count++;
    }

    state = info[0];

    while (count < 23 && info != NULL)
    {
        info = strtok(NULL, " ");
        count++;
    }

    vm_result = atoi(info);

    fclose(file);
    return;
}

void pinfo()
{
    if (!strcmp(arguments, ""))
        printf("pid -- %d\n", mainPID);

    else
    {
        char *filelink;
        filelink = (char *)malloc(1024);
        strcat(filelink, "/proc/");
        strcat(filelink, arguments);
        struct stat sts;
        if (stat(filelink, &sts) == -1 && errno == ENOENT)
        {
            printf("There's no process with given pid\n");
            return;
        }
        printf("pid -- %s\n", arguments);
    }
    proc_info();
    printf("Process Status -- %c\n", state);
    printf("memory -- %d {Virtual Memory}\n", vm_result);
    printf("Executable Path --");
    char *link;
    link = (char *)malloc(1024);
    strcpy(link, "/proc/");

    if (!strcmp(arguments, ""))
    {
        char pid_string[1024];
        itoa(mainPID, pid_string, 10);
        strcat(link, pid_string);
    }
    else
        strcat(link, arguments);

    strcat(link, "/exe");
    int i;
    char *buf;
    buf = (char *)malloc(1024);
    for (i = 0; i < 1024; i++)
        buf[i] = '\0';
    readlink(link, buf, 1024);
    printf("%s\n", buf);
    //printf("%s\n", buf);
    // printf("\n");
}