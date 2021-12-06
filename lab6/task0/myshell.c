#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define PATH_MAX 4096
#define FREE(X) if(X) free((void*)X)

void execute(cmdLine *pCmdLine){
    int err = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    if (err == -1){
        perror("execv failed");
    }
}

int main(int argc, char **argv) {
    int should_exit = 0;
    while(!should_exit){
        char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        printf("current working directory:\n%s\n", cwd);
        char sCmdLine[2048];
        fgets(sCmdLine, 2048, stdin);
        if(strcmp(sCmdLine, "quit") == 0){
            should_exit = 1;
            printf("quiting...\n");
        }
        else{
            cmdLine *pCmdLine = parseCmdLines(sCmdLine);
            execute(pCmdLine);
            FREE(pCmdLine);
        }
    }
    return 0;
}