#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define PATH_MAX 4096

void execute(cmdLine *pCmdLine){
    pid_t pid = fork();
    if(pid == 0){
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    }
    else if(pCmdLine->blocking){
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char **argv) {
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
            should_exit = 1;
        }
        else{
            execute(pCmdLine);
        }
        
        freeCmdLines(pCmdLine);
    }
    return 0;
}