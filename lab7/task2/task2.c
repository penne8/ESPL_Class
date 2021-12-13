#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

pid_t pid;
int pipefd[2];
int debug_mode = 0;


void execute(char **args)
{
    // general function - non shell
    if ((pid = fork()) == -1)
        fprintf(stderr, "ERROR: fork error\n");

    else if (pid == 0)
    { // code executed by child
        
        // execute
        execvp(args[0], args);

        // if execvp return, it failed
        printf("ERROR: %s command not found\n", args[0]);

        _exit(0);
    }
    waitpid(pid, NULL, 0);
}

void child_1_logic(){

    if(debug_mode)
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");

    close(STDOUT_FILENO);
    dup(pipefd[1]);
    close(pipefd[1]);

    if(debug_mode)
        fprintf(stderr, "(child1>going to execute cmd: …)\n");

    char *argv[] = {"ls","-l",NULL};
    execute(argv);
}

void child_2_logic(){

    if(debug_mode)
        fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe…)\n");

    close(STDIN_FILENO);
    dup(pipefd[0]);
    close(pipefd[0]);

    if(debug_mode)
        fprintf(stderr, "(child2>going to execute cmd: …)\n");

    char *argv[] = {"tail","-n","2",NULL};
    execute(argv);
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

    // Fork child 1

    if(debug_mode)
        fprintf(stderr, "(parent_process>forking…)\n");

    switch(pid_c1 = fork())
    {
        case -1:
            fprintf(stderr, "ERROR: fork error\n");
            exit(EXIT_FAILURE);
            break;
        case 0: // code executed by child 1
            child_1_logic();
            _exit(EXIT_SUCCESS);
            break;
        default:
            if(debug_mode)
                fprintf(stderr, "(parent_process>created process with id: %d)\n", pid_c1);
            break;
    }
            
    // Close write-end

    if(debug_mode)
        fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
        
    close(pipefd[1]); 

    // Fork child 2
    if(debug_mode)
        fprintf(stderr, "(parent_process>forking…)\n");

    switch(pid_c2 = fork())
    {
        case -1:
            fprintf(stderr, "ERROR: fork error\n");
            exit(EXIT_FAILURE);
            break;
        case 0: // code executed by child 1
            child_2_logic();
            _exit(EXIT_SUCCESS);
            break;
        default:
            if(debug_mode)
                fprintf(stderr, "(parent_process>created process with id: %d)\n", pid_c2);
            break;
    }

    // Close read-end file

    if(debug_mode)
       fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");

    close(pipefd[0]); 

    // Waiting

    if(debug_mode)
        fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
    waitpid(pid_c1, NULL, 0);
    waitpid(pid_c2, NULL, 0);
    
    if(debug_mode)
        fprintf(stderr, "(parent_process>exiting…)\n");

    return 0;
}