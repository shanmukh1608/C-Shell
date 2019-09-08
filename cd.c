#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "globals.h"

void cd()
{
    // if (!strcmp(Commands[currCommand].arguments[0], "") || !strcmp(Commands[currCommand].arguments[0], "~"))
    // strcpy(Commands[currCommand].arguments[0], shellHome);

    if (Commands[currCommand].argumentsIndex == 0 || (Commands[currCommand].argumentsIndex > 0 && !strcmp(Commands[currCommand].arguments[0], "~")))
    {
        Commands[currCommand].arguments[0] = (char *)malloc(1024);
        strcpy(Commands[currCommand].arguments[0], shellHome);
    }

    if (Commands[currCommand].arguments[0][0] == '~')
    {
        for (int i = 0; i < strlen(Commands[currCommand].arguments[0]) - 1; i++)
            Commands[currCommand].arguments[0][i] = Commands[currCommand].arguments[0][i + 1];

        Commands[currCommand].arguments[0][strlen(Commands[currCommand].arguments[0]) - 1] = '\0';

        char temp[1024];
        strcpy(temp, shellHome);
        strcat(temp, Commands[currCommand].arguments[0]);
        strcpy(Commands[currCommand].arguments[0], temp);
    }

    DIR *dir = opendir(Commands[currCommand].arguments[0]);

    if (dir)
    {
        /* Directory exists. */
        chdir(Commands[currCommand].arguments[0]);
    }
    else if (ENOENT == errno)
    {
        /* Directory does not exist. */
        printf("%s\n", strerror(errno));
    }
    else
    {
        printf("%s\n", strerror(errno));
    }
}