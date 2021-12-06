#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define HISTORY_SIZE 256
#define MAX_READ 2048

cmdLine** h_list = NULL;
int h_pointer = 0;

void execute(cmdLine *pCmdLine){

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

    else if(strcmp(pCmdLine->arguments[0], "history") == 0){
        for(int i=0; i<h_pointer; i++){
            printf("%d) %s\n", i, h_list[i]->arguments[0]);
        }
    }

    else{
        pid_t pid = fork();
        if(pid == 0){
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);
            // if execvp return, it failed
            printf("Unkown command");
            _exit(0);
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
        printf("MyShell~%s$", cwd);

        char sCmdLine[MAX_READ];
        fgets(sCmdLine, MAX_READ, stdin);
        h_list[h_pointer] = parseCmdLines(sCmdLine);

        if(strcmp(h_list[h_pointer]->arguments[0], "quit") == 0){
            printf("exiting...\n");
            for(int i=0; i<h_pointer; i++){
                freeCmdLines(h_list[h_pointer]);
            }
            free(h_list);
            should_exit = 1;
        }
        else{
            execute(h_list[h_pointer]);
        }

        h_pointer += 1;
    }
    return 0;
}