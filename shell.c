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

char shellPWD[1024];
char shellHome[1024];
char *shellPrompt;
char *input;

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

        else if (strlen(shellPWD) > strlen(shellHome)) //at subdirectory of home
        {
            char *temp = malloc(strlen(shellPWD));
            strcpy(temp, shellPWD);
            strcpy(shellPWD, "~");
            int homeLength = strlen(shellHome);
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
        prompt();
        printf("%s", shellPrompt);
        Input();

        // if (!strcmp(command, "cd"))
        //     cd();
        // else if (!strcmp(command, "echo"))
        //     echo();
        if (!strcmp(command, "pwd"))
            pwd();
        // else if (!strcmp(command, "ls"))
            // ls();
        // else if (!strcmp(command, "pinfo"))
            // pinfo();
        // else
            // executeCommand();
    }

    // printf("\n%s\n%s\n%s\n", command, flags, arguments);
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

    commandLoop();
    return 0;
}