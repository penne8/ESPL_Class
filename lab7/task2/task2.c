#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

pid_t pid;
int pipefd[2];
int debug_mode = 0;


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

    if(debug_mode)
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");

    close(STDOUT_FILENO);
    dup(pipefd[1]);
    close(pipefd[1]);

    if(debug_mode)
        fprintf(stderr, "(child1>going to execute cmd: …)\n");

    cmdLine *cmd = parseCmdLines("ls -l");
    execute(cmd);
    freeCmdLines(cmd);
}

void child2_logic(){

    if(debug_mode)
        fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe…)\n");

    close(STDIN_FILENO);
    dup(pipefd[0]);
    close(pipefd[0]);

    if(debug_mode)
        fprintf(stderr, "(child2>going to execute cmd: …)\n");

    cmdLine *cmd = parseCmdLines("tail -n 2");
    execute(cmd);
    freeCmdLines(cmd);
}

int main(int argc, char **argv)
{
    if(argc != 1 && (strcmp(argv[1], "-d") == 0)){
        debug_mode = 1;
    }

    pid_t pid_c1;
    pid_t pid_c2;

    if (pipe(pipefd) == -1)
    {
        fprintf(stderr, "%s", "The call to pipe() has failed.\n");
        exit(EXIT_FAILURE);
    }



    // FORIKING CHILD 1...

    if(debug_mode)
        fprintf(stderr, "(parent_process>forking…)\n");

    if ((pid_c1 = fork()) == -1)
    {
        fprintf(stderr, "ERROR: fork error\n");
        exit(EXIT_FAILURE);
    }
    else if (pid_c1 == 0)
    { // code executed by child 1
        child1_logic();
        _exit(EXIT_SUCCESS);
    }

    if(debug_mode)
        fprintf(stderr, "(parent_process>created process with id: %d)\n", pid_c1);




    if(debug_mode)
        fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");

    // close(pipefd[1]); // close write-end



    // FORIKING CHILD 2...

    if(debug_mode)
        fprintf(stderr, "(parent_process>forking…)\n");

    if ((pid_c2 = fork()) == -1)
    {
        fprintf(stderr, "ERROR: fork error\n");
        exit(EXIT_FAILURE);
    }
    else if (pid_c2 == 0)
    { // code executed by child 2
        child2_logic();
        _exit(EXIT_SUCCESS);
    }

    if(debug_mode)
        fprintf(stderr, "(parent_process>created process with id: %d)\n", pid_c2);



    if(debug_mode)
        fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n");

    // close(pipefd[0]); // close read-end file



    if(debug_mode)
        fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
    // waitpid(pid_c1, NULL, 0);
    // waitpid(pid_c2, NULL, 0);
    
    if(debug_mode)
        fprintf(stderr, "(parent_process>exiting…)\n");

    return 0;
}