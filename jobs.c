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
#include "execInput.h"

int jobIndex = 0;
struct job
{
    pid_t pid;
    char command[200];
    int stat;
    struct child *next;
} jobsArray[100];

void addJob(pid_t pid, char *command)
{
    jobsArray[jobIndex].pid = pid;
    strcpy(jobsArray[jobIndex].command, command);
    jobIndex++;
}

void deleteJob(pid_t pid)
{
    int jobNo;
    for (int i = 0; i < jobIndex; i++)
        if (jobsArray[i].pid == pid)
            jobNo = i;

    for (int i = jobNo; i < jobIndex - 1; i++)
        jobsArray[i] = jobsArray[i + 1];

    jobIndex--;
}

void jobs()
{
    char procfile[200], status[200], procString[500];
    int i;
    for (i = 0; i < jobIndex; i++)
    {
        sprintf(procfile, "/proc/%d/stat", jobsArray[i].pid);
        int fd = open(procfile, O_RDONLY);
        if (fd >= 0)
        {
            strcpy(procString, "");
            read(fd, procString, 500);
            close(fd);
            int j=0, count=0;
            while (j<500 && count <2)
            {
                if (procString[j]==' ')
                    count++;
                j++;
            }
            if (procString[j]=='S'||procString[j]=='R')
                strcpy(status, "Running");
            else
                strcpy(status, "Stopped");
        }
        else
        {
            printf("Can't access the procfile of process with pid: %d\n", jobsArray[i].pid);
            strcpy(status, "Unknown");
        }

        printf("[%d] %s %s [%d]\n", i+1, status, jobsArray[i].command, jobsArray[i].pid);
    }
}

void kjob()
{
    int jobNo = atoi(Commands[currCommand].arguments[0]);
    jobNo--;
    int signal = atoi(Commands[currCommand].arguments[1]);

    if (kill(jobsArray[jobNo].pid, signal))
        perror("kjob: ");
}

void overkill()
{
    for (int i = 0; i < jobIndex; i++)
        kill(jobsArray[i].pid, 9);
}

void fg()
{
    int jobNo = atoi(Commands[currCommand].arguments[0]);
    jobNo--;

    signal(SIGCHLD, SIG_IGN);
    kill(jobsArray[jobNo].pid, SIGCONT);

    pid_t oldBgPid = jobsArray[jobNo].pid;
    char oldBgCommand[200];
    strcpy(oldBgCommand, jobsArray[jobNo].command);
    deleteJob(oldBgPid);

    fgPid = jobsArray[jobNo].pid;

    int status;
    waitpid(jobsArray[jobNo].pid, &status, WUNTRACED);
    fgPid = 0;
    if (WIFSTOPPED(status))
        addJob(jobsArray[jobNo].pid, jobsArray[jobNo].command);
    signal(SIGCHLD, procExit);
}

void bg()
{
    int jobNo = atoi(Commands[currCommand].arguments[0]);
    jobNo--;
    kill(jobsArray[jobNo].pid, SIGCONT);
}