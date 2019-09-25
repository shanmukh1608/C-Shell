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

void jobs()
{
    for (int i=0; i<backgroundPidTop; i++)
    {
        printf("[%d] ", i+1);

        if (!backgroundStatusStack[i])
            printf("Running ");
        else
            printf("Stopped ");    
    
        printf("%s [%d]\n", backgroundProcessStack[i], backgroundPidStack[i]);
    }

}

void kjob()
{
    int jobNo=atoi(Commands[currCommand].arguments[0]);
    pid_t pid=backgroundPidStack[jobNo-1];

    int signal=atoi(Commands[currCommand].arguments[1]);
    
    if (kill(pid, signal))
        perror("kjob: ");
}

