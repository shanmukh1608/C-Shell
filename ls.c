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

void full_ls_file(char *filename, int a, int max)
{
    struct stat fileStat;

    if (stat(filename, &fileStat) < 0)
    {
        perror("ls: ");
        return;
    }

    char permissions[10] = "";
    strcat(permissions, ((S_ISDIR(fileStat.st_mode)) ? "d" : "-"));
    strcat(permissions, ((fileStat.st_mode & S_IRUSR) ? "r" : "-"));
    strcat(permissions, ((fileStat.st_mode & S_IWUSR) ? "w" : "-"));
    strcat(permissions, ((fileStat.st_mode & S_IXUSR) ? "x" : "-"));
    if (!strcmp(permissions, ""))
        return;

    strcat(permissions, ((fileStat.st_mode & S_IRGRP) ? "r" : "-"));
    strcat(permissions, ((fileStat.st_mode & S_IWGRP) ? "w" : "-"));
    strcat(permissions, ((fileStat.st_mode & S_IXGRP) ? "x" : "-"));
    if (!strcmp(permissions, ""))
        return;

    strcat(permissions, ((fileStat.st_mode & S_IROTH) ? "r" : "-"));
    strcat(permissions, ((fileStat.st_mode & S_IWOTH) ? "w" : "-"));
    strcat(permissions, ((fileStat.st_mode & S_IXOTH) ? "x" : "-"));
    if (!strcmp(permissions, ""))
        return;

    time_t *t;
    t = malloc(1024);
    *t = fileStat.st_mtime;

    if (!strcmp(permissions, ""))
        return;
    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(fileStat.st_gid);
    pwd = getpwuid(fileStat.st_uid);

    if (!strcmp(permissions, ""))
        return;

    struct tm tm = *localtime(t);
    Mon(tm.tm_mon + 1);

    printf("%s %2ld %s %s %*ld %s %d %d:%02d %s\n", permissions, fileStat.st_nlink, pwd->pw_name, grp->gr_name, max, fileStat.st_size, mon, tm.tm_mday, tm.tm_hour, tm.tm_min, filename);
}

void full_ls(int a)
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    DIR *curdir1 = opendir(Commands[currCommand].arguments[0]);
    struct dirent *curfile1;
    int max = 0; //store digits of maximum size
    chdir(Commands[currCommand].arguments[0]);
    while ((curfile1 = readdir(curdir1)) != NULL)
    {
        struct stat fileStat1;
        if (stat(curfile1->d_name, &fileStat1) < 0)
            return;
        max = 0;
        if (digits(fileStat1.st_size) > max)
            max = digits(fileStat1.st_size);
    }
    if (strcmp(cwd, ""))
        chdir(cwd);

    struct dirent *curfile;
    DIR *curdir = opendir(Commands[currCommand].arguments[0]);
    if (strcmp(Commands[currCommand].arguments[0], ""))
        chdir(Commands[currCommand].arguments[0]);

    while ((curfile = readdir(curdir)) != NULL)
    {
        if ((isHiddenFile(curfile->d_name)) && (!a))
            continue;

        full_ls_file(curfile->d_name, a, max);
    }
    if (strcmp(Commands[currCommand].arguments[0], ""))
        chdir(Commands[currCommand].arguments[0]);
}

void regular_ls(int a)
{
    DIR *curdir = opendir(Commands[currCommand].arguments[0]);
    struct dirent *curfile;
    while ((curfile = readdir(curdir)) != NULL)
    {
        if ((isHiddenFile(curfile->d_name)) && (!a))
            continue;
        printf("%s   ", curfile->d_name);
    }
    printf("\n");
}

void ls()
{
    if (Commands[currCommand].argumentsIndex == 0)
    {
        char cur[1024];
        if (getcwd(cur, sizeof(cur)) == NULL)
            perror("getcwd() error");
        Commands[currCommand].arguments[0] = (char *)malloc(1024);
        strcpy(Commands[currCommand].arguments[0], cur);
    }

    else if (!strcmp(Commands[currCommand].arguments[0], "~"))
        strcpy(Commands[currCommand].arguments[0], shellHome);

    else if (Commands[currCommand].arguments[0][0] == '~')
    {
        for (int i = 0; i < strlen(Commands[currCommand].arguments[0]) - 1; i++)
            Commands[currCommand].arguments[0][i] = Commands[currCommand].arguments[0][i + 1];
        Commands[currCommand].arguments[0][strlen(Commands[currCommand].arguments[0]) - 1] = '\0';

        char temp[1024];
        strcpy(temp, shellHome);
        strcat(temp, Commands[currCommand].arguments[0]);
        strcpy(Commands[currCommand].arguments[0], temp);
    }

    int l = 0, a = 0;
    if (Commands[currCommand].flagsIndex == 2)
        if ((!strcmp(Commands[currCommand].flags[0], "-l") && !strcmp(Commands[currCommand].flags[1], "-a")) || (!strcmp(Commands[currCommand].flags[0], "-a") && !strcmp(Commands[currCommand].flags[1], "-l")))
            l = 1, a = 1;
    else if (Commands[currCommand].flagsIndex == 1)
    { 
        if (!strcmp(Commands[currCommand].flags[0], "-la") || !strcmp(Commands[currCommand].flags[0], "-al"))
            l = 1, a = 1;
        else if (!strcmp(Commands[currCommand].flags[0], "-l"))
            l = 1;
        else if (!strcmp(Commands[currCommand].flags[0], "-a"))
            a = 1;
    }

    struct stat path_stat;
    stat(Commands[currCommand].arguments[0], &path_stat);

    if (S_ISDIR(path_stat.st_mode)) //if ls is run on directory
    {
        if (l)
            full_ls(a);
        else
            regular_ls(a);
    }
    else if (isFile(Commands[currCommand].arguments[0]))
        if (l)
            full_ls_file(Commands[currCommand].arguments[0], a, 10);
        else
            printf("%s\n", Commands[currCommand].arguments[0]);
    else
        perror("");
}