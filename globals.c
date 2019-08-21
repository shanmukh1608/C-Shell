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

int isNOTDIR(const char* name)
{
    DIR* directory = opendir(name);

    if(directory != NULL)
    {
     closedir(directory);
     return 0;
    }

    if(errno == ENOTDIR)
    {
     return 1;
    }

    return -1;
}

int isFile(char* file){
    struct stat fil;
    return (stat(file, &fil) == 0);
}

int isHiddenFile(char* file){
	return (file[0]==46);
}

void Mon(int num){
	switch (num)
	{
		case 1: strcpy(mon, "Jan");
				break;
		case 2: strcpy(mon, "Feb");
				break;
		case 3: strcpy(mon, "Mar");
				break;
		case 4: strcpy(mon, "Apr");
				break;
		case 5: strcpy(mon, "May");
				break;
		case 6: strcpy(mon, "Jun");
				break;
		case 7: strcpy(mon, "Jul");
				break;
		case 8: strcpy(mon, "Aug");
				break;
		case 9: strcpy(mon, "Sep");
				break;
		case 10: strcpy(mon, "Oct");
				break;
		case 11: strcpy(mon, "Nov");
				break;
		case 12: strcpy(mon, "Dec");
				break;
	}
}

int digits(int num)
{
	int count = 0;
	while(num != 0)
    {
        count++;
        num /= 10;
    }
    return count;
}
