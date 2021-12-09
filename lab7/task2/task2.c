#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

pid_t pid;
int pipefd[2];
char buf[32];


void execute(cmdLine *pCmdLine)
{
    // general function - non shell
    if ((pid = fork()) == -1)
        fprintf(stderr, "ERROR: fork error\n");

    else if (pid == 0)
    { // code executed by child

        // change input stream
        if(pCmdLine->inputRedirect != NULL){
            freopen(pCmdLine->inputRedirect, "r", stdin);
        }

        // change output stream
        if(pCmdLine->outputRedirect != NULL){
            freopen(pCmdLine->outputRedirect, "w", stdout);
        }

        // execute
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);

        // if execvp return, it failed
        printf("ERROR: %s command not found\n", pCmdLine->arguments[0]);

        freeCmdLines(pCmdLine);
        _exit(0);
    }
    else if (pCmdLine->blocking)
    { // code executed by parent
        waitpid(pid, NULL, 0);
    }
}

void child1_logic(){
    close(STDOUT_FILENO);
    int dupfd = dup(pipefd[0]);
    close(dupfd);
    cmdLine *cmd = parseCmdLines("ls -l");
    execute(cmd);
    freeCmdLines(cmd);
}

void child2_logic(){
    close(STDIN_FILENO);
    int dupfd = dup(pipefd[1]);
    close(dupfd);
    cmdLine *cmd = parseCmdLines("tail -n 2");
    execute(cmd);
    freeCmdLines(cmd);
}

int main(int argc, char **argv)
{
    if ((pid = fork()) == -1)
        fprintf(stderr, "ERROR: fork error\n");

    else if (pid == 0)
    { // code executed by child 
        child1_logic();
        _exit(EXIT_SUCCESS);
    }
    close(pipefd[1]);

    if ((pid = fork()) == -1)
        fprintf(stderr, "ERROR: fork error\n");

    else if (pid == 0)
    { // code executed by child 
        child2_logic();
        _exit(EXIT_SUCCESS);
    }
    close(pipefd[0]);

    waitpid(pid, NULL, 0);
    waitpid(pid, NULL, 0);
    
    return 0;
}