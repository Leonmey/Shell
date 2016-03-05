#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define MAX_INPUT 256

void tokenizeInput(char* input, char** tokens);
int addToHistory(char** history,int hisCounter,char* input);
void printHistory(char** history,int hisCounter,char* numHistory);
int splitCmds(char** tokens,char**cmds,int* inFd, int* outFd);
void splitPipeCmds(char** cmds, char** cmds2, int pipenum);
void allocateInitCmds(char*** cmdsArg);
void freeMemCmds(char*** cmdsArg);
