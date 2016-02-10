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

/*//fill the cmdsArgs with the correct cmds and args
			int j = 0, k =0;
			for(int i = 0; tokens[i] != NULL; i++){
				if(strcmp(tokens[i],"|") == 0 ){
					cmdsArg[j][k] = NULL;
					if(strcmp(cmdsArg[3][0],"|") == 0 )
						j = 3;
					else if (strcmp(cmdsArg[4][0],"|") == 0 )
						j = 4;
					else if (strcmp(cmdsArg[5][0],"|") == 0 )
						j = 5;
					else{
						perror("Only supports 3 pipes");
						flag = 1;
						j = 2;
					}
					k = 0;
				}
				else if(strcmp(tokens[i],"<") == 0 ){
					cmdsArg[j][k] = NULL;

					if(cmdsArg[1][0] == NULL )
						j = 1;
					else{
						perror("Only supports 1 redirect in");
						flag = 1;
						j = 1;
					}
					i++;
					k = 0;
				}
				else if(strcmp(tokens[i],">") == 0 ){
					cmdsArg[j][k] = NULL;
					if(cmdsArg[2][0] == NULL)
						j = 2;
					 else{
						perror("Only supports 1 redirect out");
						flag = 1;
						j = 2;
					}
					i++;
					k = 0;
				}
				cmdsArg[j][k] = strdup(tokens[i]);
				k++;
			}
			cmdsArg[j][k] = NULL;

			// int counter;
			// for (counter = 0; cmdsArg[0][counter] != NULL; counter++){

			// }


			printf("this is it:%s\n",cmdsArg[1][0]);
			printf("wait a sec: %s %s\n",cmdsArg[0][0],cmdsArg[0][1]);
			printf("flags: %d:%d:%d\n",redirI,redirO,pipes);*/