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

int noOfJobs = 0;
typedef struct child
{
    pid_t pid;
    char command[200];
    int stat;
    struct child *next;
} * job;

job head = NULL;

job createNewNode()
{
    job temp;
    temp = (job)malloc(sizeof(struct child));
    // temp->command = (char *)malloc(1024);
    temp->next = NULL;
    return temp;
}

void addJob(pid_t pid, char *command)
{
    job new = createNewNode();
    new->pid = pid;
    strcpy(new->command, command);
    if (head == NULL)
        head = new;
    else
    {
        job temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = new;
    }

    noOfJobs++;
    return;
}

void deleteJob(pid_t pid)
{
    if (head->pid == pid)
    {
        job temp = head;
        head = head->next;
        noOfJobs--;
        return temp;
    }

    job itr = head;
    while (itr->next != NULL)
    {
        if (itr->next->pid != pid)
        {
            job temp = itr->next;
            itr->next = temp->next;
            noOfJobs--;
            return temp;
        }
        itr = itr->next;
    }
}

void jobs()
{
    job itr = head;
    int count = 1;
    char procfile[200], status[200];
    while (itr)
    {
        sprintf(procfile, "/proc/%d/stat", itr->pid);
        int fd = open(procfile, O_RDONLY);
        if (fd == -1)
        {
            printf("Can't access the STATE information of pid: %d.\n", itr->pid);
            strcpy(status, "UNKNOWN STATE");
        }
        else
        {
            char temp[500];
            read(fd, temp, 500);
            close(fd);
            int total = 0;
            for (int i = 0; i < 500; i++)
            {
                if (temp[i] == ' ')
                {
                    total++;
                    if (total == 2)
                    {
                        if (temp[i + 1] == 'S' || temp[i + 1] == 'R')
                        {
                            strcpy(status, "Running");
                        }
                        else if (temp[i + 1] == 'T')
                        {
                            strcpy(status, "Stopped");
                        }
                        else
                        {
                            status[0] = temp[i + 1];
                            status[1] = '\0';
                        }
                        break;
                    }
                }
            }
        }
        printf("[%d] %s %s [%d]\n", count, status, itr->command, itr->pid);
        count++;
        itr = itr->next;
    }
    return;
}

void kjob()
{
    int jobNo = atoi(Commands[currCommand].arguments[0]);
    int signal = atoi(Commands[currCommand].arguments[1]);

    int index = 1;
    job itr = head;
    while (index != jobNo)
    {
        if (!itr)
        {
            printf("Error occured while iterating through the jobs\n");
            return;
        }
        index++;
        itr = itr->next;
    }

    if (kill(itr->pid, signal))
        perror("kjob: ");
}

void overkill()
{
    job itr = head;
    kill(head->pid, 9);

    while (itr->next != NULL)
    {
        itr = itr->next;
        kill(itr->pid, 9);
    }
}

void fg()
{
    
    int jobNo = atoi(Commands[currCommand].arguments[0]);
    int index = 1;
    job itr = head;

    while (index != jobNo)
    {
        if (!itr)
        {
            printf("Error occured while iterating through the jobs\n");
            return;
        }
        index++;
        itr = itr->next;
    }

    if (!itr)
    {
        printf("Error occured while iterating through the jobs\n");
        return;
    }

    signal(SIGCHLD, SIG_IGN);
    kill(itr->pid, SIGCONT);
    fgPid = itr->pid;

    struct child p = *itr;
    deleteJob(itr->pid);
    free(itr);
    int status;
    waitpid(p.pid, &status, WUNTRACED);
    fgPid = 0;
    if (WIFSTOPPED(status))
        addJob(p.pid, p.command);
    signal(SIGCHLD, procExit);
}

void bg()
{
    int jobNo = atoi(Commands[currCommand].arguments[0]);
    if (noOfJobs < jobNo)
    {
        printf("There are only %d jobs in the background currently\n", noOfJobs);
        return;
    }
    int index = 1;
    job itr = head;
    while (index != jobNo)
    {
        if (!itr)
        {
            printf("Error occured while iterating through the jobs\n");
            return;
        }
        index++;
        itr = itr->next;
    }
    if (!itr)
    {
        printf("Error occured while iterating through the jobs\n");
        return;
    }

    kill(itr->pid, SIGCONT);
}