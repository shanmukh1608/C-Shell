#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

extern char *input;
extern char *command;
extern char *flags;
extern char *arguments;
extern char shellHome[1024];
extern char mon[4];
extern char shellPWD[1024];
extern char *shellPrompt;
extern char *currCommand;
extern int main_pid;

int isNOTDIR(const char *name);
int isFile(char *file);
int isHiddenFile(char *file);
void Mon(int num);
int digits(int num);
int is_large(char c);
void swap(char *x, char *y);
char* itoa(int value, char* buffer, int base);
char* reverse(char *buffer, int i, int j);

#endif