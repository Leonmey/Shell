#include "shell.h"

int main()
{
	char input[MAX_INPUT];
	char* tokens[MAX_INPUT];
	char* history[MAX_INPUT];
	char* cmds[MAX_INPUT], *cmds2[MAX_INPUT];
	int hisCounter = 0, flag = 0, numCmds = 0;
	int* fds;
	pid_t pid2;
	pid_t pid;

	while(1){

		int inFd = 0, outFd=0;
		printf("LMeyer> ");

		//get the input line from the user
		fgets(input,MAX_INPUT,stdin);

		//add the input to the history
		hisCounter = addToHistory(history,hisCounter,input);

		if(strcmp(input,"exit\n") == 0){
			break;
		}

		//tokenize the input into the tokens array
		tokenizeInput(input,tokens);

		if(strcmp(tokens[0],"history") == 0){
			printHistory(history,hisCounter,tokens[1]);
			flag = 1;
		}
		//blank entry just loops back to get next input
		else if (strcmp(tokens[0],"") == 0)
			flag = 1;

		//flag used for blank and history
		if(!flag){
			pid = fork();

			//error creating new pid
			if(pid<0)
			{
				perror("fork error");
				exit(pid);
			}

			//parent
			if (pid>0){
				wait(NULL);
			}

			//child
			if(pid == 0){

				//split up commands
				numCmds = splitCmds(tokens,cmds,&inFd,&outFd);

				//create all of the pipes
				if(numCmds > 1){
					fds = (int*)malloc(sizeof(int)*(numCmds*2));
					for(int i = 0; i < numCmds; i++){
						if(pipe(fds + i*2) < 0){
							perror("pipe failed");
							exit(EXIT_FAILURE);
						}
					}
				}

				//redirection in
				if(inFd > 0){
					if(dup2(inFd,STDIN_FILENO)<0){
						perror("cannot dup stdin");
						exit(1);
					}
					close(inFd);
				}

				//redirection out
				if(outFd > 0){
					if(dup2(outFd,STDOUT_FILENO)<0){
						perror("cannot dup stdout");
						exit(1);
					}
					close(outFd);
				}

				//command with no pipes
				if(numCmds == 1){;
					if(execvp(*cmds,cmds) == -1){
						printf("Error executing command1: %s %s\n",cmds[0],cmds[1]);
						exit(1);
					}
				}
				//pipe or redirect
				else{
					for(int i =0; i<numCmds; i++){

						//FORK HERE
						if((pid2 = fork())<0){
							perror("fork error");
							exit(1);
						}
						else if (pid2 > 0){
							//Do nothing
						}
						else if(pid2 == 0){
							//get the corrrect command for given pipe
							splitPipeCmds(cmds,cmds2,i);

							//Not first command
							if(i != 0){
								if(dup2(fds[(i-1)*2],STDIN_FILENO) < 0){
									perror("error with dup");
									exit(EXIT_FAILURE);
								}
							}

							//Not last cmd
							if(i < (numCmds-1)){
								if(dup2(fds[i*2+1],STDOUT_FILENO) < 0){
									perror("error with dup");
									exit(EXIT_FAILURE);
								}
							}

							//close all the fds
							for(int j=0;j<2*numCmds;j++){
								close(fds[j]);
							}

							//execute commands
							if(execvp(*cmds2,cmds2)==-1){
								printf("Error executing command2: %s %s\n",cmds[0],cmds[1]);
								exit(EXIT_FAILURE);
							}
						}
						
					}
					//only parent gets here.
					//closes all copies of fd
					for(int i = 0; i<2*numCmds; i++){
						close(fds[i]);
					}
					//wait for all the children
					for(int i=0;i<numCmds + 1; i++){
						wait(NULL);	
					}

					//exit back to original parent and wait for next input
					exit(0);
				}
			}

		}
		flag = 0;

	}
	//free the allocated history
	for(int i = 0;i <hisCounter; i++){
		free(history[i]);
	}
	free(fds);
	
	return 0;
}

//takes the users input and splts up the words at white space
void tokenizeInput(char* input, char** tokens){
	int i = 0;
	tokens[i] = strtok(input," ");

	do{
		i++;
		tokens[i] = strtok(NULL, " ");
	}while(tokens[i] != NULL);
	//remove the extra newline character that fgets adds
	strtok(tokens[i-1], "\n");
}

//add an input to the history
int addToHistory(char** history,int hisCounter,char* input){
	history[hisCounter] = strdup(input);
	return ++hisCounter;
}

//print the history of commands, default is last 10 commands
void printHistory(char** history,int hisCounter, char* numHistory){
	int historyItem;
	// sets the history counter to the correct place
	if(numHistory != NULL){
		int num = atoi(numHistory);
		if(num == 0)
			historyItem = hisCounter;
		else{
			historyItem = hisCounter - num;
			if(historyItem <0)
				historyItem = 0;
		}
	}
	else if(hisCounter >= 10){
		historyItem = hisCounter - 10;
	}
	else{
		historyItem = 0;
	}

	//prints the history
	for(int i = 1;historyItem < hisCounter;i++,historyItem++){
		printf("%d %s",i,history[historyItem]);
	}
}

int splitCmds(char** tokens,char** cmds,int* inFd, int* outFd){
	int numCmds = 1;
	int i,f;
	for(i = 0, f = 0; tokens[i] != NULL;i++){
			//if redirect in then open the in file
		if(strcmp(tokens[i],"<") == 0){
			if((*inFd = open(tokens[++i],O_RDONLY)) < 0){
				printf("could not open file %s",tokens[i]);
				exit(1);
			}
		}
			//if redirect out then open the out file
		else if (strcmp(tokens[i],">") == 0){
			if((*outFd = open(tokens[++i], O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) < 0){
				printf("could not open file %s", tokens[i]);
				exit(1);
			}
			f = 1;	
		}
			//if pipe then add another command, include the pipe in the cmds list
		else if(strcmp(tokens[i],"|") == 0){
			numCmds++;
		}
		if(!f){
			cmds[i] = tokens[i];
		}
		f = 0;
	}
	cmds[i] = NULL;
	return numCmds;
}

void splitPipeCmds(char** cmds, char** cmds2, int pipenum){
	int j = 0;
	for (int i = 0; cmds[i] != NULL; i++){
		if(strcmp(cmds[i],"|") == 0){
			i++;
			pipenum --;
			if(pipenum == -1)
				break;
		}
		if(pipenum == 0){
			cmds2[j] = cmds[i];
			j++;
		}
	}
	//add null to the end
	cmds2[j] = NULL;
}