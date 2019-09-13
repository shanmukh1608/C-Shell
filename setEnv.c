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

void setEnv()
{
    if (Commands[currCommand].argumentsIndex == 0 || Commands[currCommand].argumentsIndex > 2)
    {
        printf("Error, incorrect number of arguments to setenv.\n");
        return;
    }

    if (setenv(Commands[currCommand].arguments[0], Commands[currCommand].arguments[1], 1))
        perror("setenv: ");
}

void unsetEnv()
{
    if (Commands[currCommand].argumentsIndex == 0)
    {
        printf("Error, incorrect number of arguments to setenv.\n");
        return;
    }

    if (unsetenv(Commands[currCommand].arguments[0]))
        perror("unsetenv: ");
}