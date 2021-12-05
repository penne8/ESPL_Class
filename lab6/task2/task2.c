#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define PATH_MAX 4096
#define FREE(X) if(X) free((void*)X)
#define HISTORY_SIZE 256
#define COMMAND_SIZE 32

cmdLine** h_list = NULL;
int h_pointer = 0;

void execute(cmdLine *pCmdLine){

    // check if first char of first argument is '!'
    if(pCmdLine->arguments[0][0] == 33){
        int restored_command_index = atoi(pCmdLine->arguments[0]+sizeof(char));
        FREE(pCmdLine);
        pCmdLine = h_list[restored_command_index];
    }

    // set history command
    h_list[h_pointer] = pCmdLine;
    h_pointer += 1;

    // cd function
    char err = 0;
    if(strcmp(pCmdLine->arguments[0], "cd") == 0){
        if(pCmdLine->argCount == 1){
            err = chdir(getenv("HOME"));
        }
        else{
            err = chdir(pCmdLine->arguments[1]);
        }

        if(err)
            fprintf(stderr, "error trying to cd");
    }

    // history function
    else if(strcmp(pCmdLine->arguments[0], "history") == 0){
        printf("\n");
        for(int i=0; i<h_pointer; i++){
            printf("%d) %s\n", i, h_list[i]->arguments[0]);
        }
    }

    // general function - non shell
    else{
        pid_t pid = fork();
        if(pid == 0){
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        }
        else if(pCmdLine->blocking){
            waitpid(pid, NULL, 0);
        } 
    }
}

int main(int argc, char **argv) {
    h_list = (cmdLine **) malloc(sizeof(cmdLine *)*HISTORY_SIZE);

    int should_exit = 0;
    while(!should_exit){

        char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        printf("\ncurrent working directory:\n%s\n", cwd);

        char sCmdLine[2048];
        fgets(sCmdLine, 2048, stdin);
        cmdLine *pCmdLine = parseCmdLines(sCmdLine);

        if(strcmp(pCmdLine->arguments[0], "quit") == 0){
            printf("exiting...\n");
            for(int i=0; i<h_pointer; i++){
                FREE(h_list[h_pointer]);
            }
            FREE(h_list);
            should_exit = 1;
        }
        else{
            execute(pCmdLine);
        }
    }
    return 0;
}