#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "globals.h"
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <termios.h>

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state)
{
    struct termios ttystate;

    tcgetattr(STDIN_FILENO, &ttystate); //get the terminal state

    if (state == 1)
    {
        ttystate.c_lflag &= ~ICANON; //turn off canonical mode
        ttystate.c_cc[VMIN] = 1;     //minimum of number input read.
    }
    else if (state == 0)
        ttystate.c_lflag |= ICANON; //turn on canonical mode

    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void nightswatch()
{
    char timestr[10];
    size_t i = 0;

    while (!isDigit(flags[i]))
        i++;
    while (i < strlen(flags))
    {
        char cToStr[2];
        cToStr[1] = '\0';
        cToStr[0] = flags[i];
        strcat(timestr, cToStr);
        i++;
    }

    int time = atoi(timestr);

    fd_set input_set;
    struct timeval timeout;

    /* Empty the FD Set */
    FD_ZERO(&input_set);
    /* Listen to the input descriptor */
    FD_SET(STDIN_FILENO, &input_set);

    if (!strcmp(arguments, "interrupt"))
    {
        // Interupt
        printf("0\tCPU0\tCPU1\tCPU2\tCPU3\tCPU4\tCPU5\tCPU6\tCPU7\n");
        int k = 1, status = 0;

        nonblock(1);
        while (!status)
        {
            FILE *interrupt = fopen("/proc/interrupts", "r");
            ssize_t reads;
            size_t len = 0;
            char *line = NULL;

            if (interrupt == NULL)
            {
                perror("Error opening interrupt file: ");
                return;
            }

            int i = 0;

            while (i < 3 && (reads = getline(&line, &len, interrupt)) != -1)
                i++;

            long long int cpu0, cpu1, cpu2, cpu3, cpu4, cpu5, cpu6, cpu7;
            // printf("%s\n", line);

            sscanf(line, "%*lld: %lld %lld %lld %lld %lld %lld %lld %lld", &cpu0, &cpu1, &cpu2, &cpu3, &cpu4, &cpu5, &cpu6, &cpu7);

            fprintf(stderr, "%d\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\n", k, cpu0, cpu1, cpu2, cpu3, cpu4, cpu5, cpu6, cpu7);
            k++;

            fclose(interrupt);

            timeout.tv_sec = time; // time seconds
            timeout.tv_usec = 0;   // 0 milliseconds
            select(1, &input_set, NULL, NULL, &timeout);

            status = kbhit();
            if (status != 0)
            {
                char c = fgetc(stdin);
                if (c == 'q')
                    status = 1;
                else
                    status = 0;
            }
        }
        nonblock(0);
        return;
    }

    if (!strcmp(arguments, "dirty"))
    {
        // dirty
        int status = 0;
        nonblock(1);

        while (!status)
        {
            FILE *meminfo = fopen("/proc/meminfo", "r");
            ssize_t reads;
            size_t len = 0;
            char *line = NULL;

            if (meminfo == NULL)
            {
                perror("Error opening meminfo file: ");
                return;
            }

            int i = 0;

            while (i < 17 && (reads = getline(&line, &len, meminfo)) != -1)
                i++;

            printf("%s", line);

            fclose(meminfo);

            timeout.tv_sec = time; // time seconds
            timeout.tv_usec = 0;   // 0 milliseconds
            select(1, &input_set, NULL, NULL, &timeout);
            status = kbhit();
            if (status != 0)
            {
                char c = fgetc(stdin);
                if (c == 'q')
                    status = 1;
                else
                    status = 0;
            }
        }
        nonblock(0);
        return;
    }

    return;
}
