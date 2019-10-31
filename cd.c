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
    if (!strcmp(arguments, "") || !strcmp(arguments, "~"))
        strcpy(arguments, shellHome);

    if (arguments[0] == '~')
    {
        for (unsigned long i = 0; i < strlen(arguments) - 1; i++)
            arguments[i] = arguments[i + 1];
        arguments[strlen(arguments) - 1] = '\0';

        char temp[1024];
        strcpy(temp, shellHome);
        strcat(temp, arguments);
        strcpy(arguments, temp);
    }

    DIR *dir = opendir(arguments);

    if (dir)
    {
        /* Directory exists. */
        chdir(arguments);
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
