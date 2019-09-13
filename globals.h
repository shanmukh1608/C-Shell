#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

extern char *input;
extern char *command;
extern char *flags;
struct commandStruct
{
    char *command, *flags[1024], *arguments[1024], *outputFile, *inputFile;
    int flagsIndex, argumentsIndex, outputFd, inputFd, backgroundFlag, errorFlag;
} Commands[1024];

extern char shellHome[1024];
extern char mon[4];
extern char shellPWD[1024];
extern char *shellPrompt;

extern char *sepInput;
extern char *currInput;
extern int currCommand;

extern int mainPID;
extern int pidStack[1024];
extern char processStack[1024][1024];
extern int pidTop;

extern int backgroundPidStack[1024];
extern int backgroundStatusStack[1024];
extern char backgroundProcessStack[1024][1024];
extern int backgroundPidTop;

extern char pipeSeparated[1024][1024];
extern int pipeIndex;
extern int pipes[1024];
extern int savestdin;
extern int savestdout;

int isNOTDIR(const char *name);
int isFile(char *file);
int isHiddenFile(char *file);
int isDigit(char c);
void Mon(int num);
int digits(int num);
int is_large(char c);
void swap(char *x, char *y);
char *itoa(int value, char *buffer, int base);
char *reverse(char *buffer, int i, int j);

#endif