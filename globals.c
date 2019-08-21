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

int is_large(char c)
{
	return ((c<91)&&(64<c));
}

void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
}

char* reverse(char *buffer, int i, int j)
{
	while (i < j)
		swap(&buffer[i++], &buffer[j--]);

	return buffer;
}

char* itoa(int value, char* buffer, int base)
{
	if (base < 2 || base > 32)
		return buffer;
	int n = abs(value);

	int i = 0;
	while (n)
	{
		int r = n % base;

		if (r >= 10) 
			buffer[i++] = 65 + (r - 10);
		else
			buffer[i++] = 48 + r;

		n = n / base;
	}

	if (i == 0)
		buffer[i++] = '0';
	if (value < 0 && base == 10)
		buffer[i++] = '-';

	buffer[i] = '\0';
	return reverse(buffer, 0, i - 1);
}