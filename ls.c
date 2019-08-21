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

void full_ls_file(char* filename, int a, int max)
{
    struct stat fileStat;
    
    if(stat(filename,&fileStat) < 0)    
        return;

    char permissions[10] = "";
    strcat(permissions, ( (S_ISDIR(fileStat.st_mode)) ? "d" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IRUSR) ? "r" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IWUSR) ? "w" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IXUSR) ? "x" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IRGRP) ? "r" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IWGRP) ? "w" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IXGRP) ? "x" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IROTH) ? "r" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IWOTH) ? "w" : "-"));
    strcat(permissions, ( (fileStat.st_mode & S_IXOTH) ? "x" : "-"));

    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(fileStat.st_gid);
    pwd = getpwuid(fileStat.st_uid);
    time_t *t;
    t = malloc(1024);
    *t = fileStat.st_mtime;
    struct tm tm = *localtime(t);
    Mon(tm.tm_mon+1);
    printf("%s %2ld %s %s %*ld %s %d %d:%02d %s\n", permissions, fileStat.st_nlink, pwd->pw_name, grp->gr_name, max, fileStat.st_size, mon, tm.tm_mday, tm.tm_hour, tm.tm_min, filename);
}

void full_ls(int a){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    DIR * curdir1 = opendir(arguments);
    struct dirent *curfile1;
    int max = 0; //store digits of maximum size
    chdir(arguments);
    while((curfile1 = readdir(curdir1)) != NULL)
    {
        struct stat fileStat1;
        if(stat(curfile1->d_name,&fileStat1) < 0)    
            return;
        if (digits(fileStat1.st_size)>max)
            max = digits(fileStat1.st_size);
    }
    chdir(cwd);

    struct dirent *curfile;
    DIR * curdir = opendir(arguments);
    chdir(arguments);
    while((curfile = readdir(curdir)) != NULL)
    {
        if ((isHiddenFile(curfile->d_name))&&(!a))
            continue;
        full_ls_file(curfile->d_name, a, max);
    }
    chdir(cwd);
}

void regular_ls(int a)
{
    
    DIR *curdir = opendir(arguments);
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
    if (!strcmp(arguments, ""))
    {
        char cur[1024];
        if (getcwd(cur, sizeof(cur)) == NULL)
            perror("getcwd() error");
        strcpy(arguments, cur);
    }

    else if (!strcmp(arguments, "~"))
        strcpy(arguments, shellHome);

    else if (arguments[0] == '~')
    {
        for (int i = 0; i < strlen(arguments) - 1; i++)
            arguments[i] = arguments[i + 1];
        arguments[strlen(arguments) - 1] = '\0';

        char temp[1024];
        strcpy(temp, shellHome);
        strcat(temp, arguments);
        strcpy(arguments, temp);
    }

    int l = 0, a = 0;

    if (!strcmp(flags, "-la ") || !strcmp(flags, "-al ") || !strcmp(flags, "-l -a ") || !strcmp(flags, "-a -l "))
        l = 1, a = 1;
    else if (!strcmp(flags, "-l "))
        l = 1;
    else if (!strcmp(flags, "-a "))
        a = 1;

    struct stat path_stat;
    stat(arguments, &path_stat);

    if (S_ISDIR(path_stat.st_mode)) //if ls is run on directory
    {        
        if (l)
            full_ls(a);
        else
            regular_ls(a);
    }
    else if (isFile(arguments))
        if (l)
            full_ls_file(arguments, a, 10);
        else
            printf("%s\n", arguments);
    else
        perror("");
}