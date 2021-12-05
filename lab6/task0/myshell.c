#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define PATH_MAX 4096
#define FREE(X) if(X) free((void*)X)

int shell_command(cmdLine *pCmdLine){
    char *command = pCmdLine->arguments[0];
    if(strcmp(command, "cd")==0){
        if(pCmdLine->argCount == 1)
            chdir(getenv("HOME"));
        else
            chdir(pCmdLine->arguments[1]);
            char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        printf("\ncurrent working directory:\n%s\n", cwd);
        return 1;
    }

    return 0;
}

void execute(cmdLine *pCmdLine){
    if(!shell_command(pCmdLine)){
        pid_t pid = fork();
        if(pid > 0){
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        }
        else{
            if(pCmdLine->blocking){
                waitpid(pid, NULL, 0);
            }

        }
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
        FREE(pCmdLine);
    }
    return 0;
}