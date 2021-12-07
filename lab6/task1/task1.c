#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define HISTORY_SIZE 6
#define MAX_READ 2048
#define TRUE 1
#define ADD_HISTORY 1
#define DONT_ADD_HISTORY 0

pid_t pid;
char* h_array[HISTORY_SIZE];    // history list of commands
int h_count = 0;                // number of total commands
int h_pointer = 0;              // current command index in history list

void free_history(){
    for (int i = 0; i < HISTORY_SIZE; i++)
        free(h_array[i]);
}

void add_history(char *command){
    free(h_array[h_pointer]);
    h_array[h_pointer] = (char *) malloc(MAX_READ);
    strcpy(h_array[h_pointer], command);

    h_pointer++;
    h_pointer %= HISTORY_SIZE;
    h_count++;
}

void print_history(){
    // current command pointer inside h_array
    int h_curr_pointer = (h_count < HISTORY_SIZE) ? 0 : ((h_pointer + 1) %  HISTORY_SIZE);
    // current command index in shell
    int h_command_index = (h_count < HISTORY_SIZE) ? 0 : (h_count - HISTORY_SIZE + 1);

    /*
    continue untill curr h_position.
    (over all, HISTORY_SIZE-1 commands will be printed, all commands
    in h_array, but without the oldest command that the calling history
    command will replace in the h_array)
    */
    while(h_command_index != h_count){

        if(h_array[h_curr_pointer]){
            printf("%d) %s\n", h_command_index, h_array[h_curr_pointer]);
        }

        h_curr_pointer++;
        h_curr_pointer %= HISTORY_SIZE;
        h_command_index++;
    }
}

void execute(cmdLine* pCmdLine){

    // cd function
    if(strcmp(pCmdLine->arguments[0], "cd") == 0){
        char err = 1;

        if(pCmdLine->argCount == 1){
            err = chdir(getenv("HOME"));
        }
        else if(pCmdLine->argCount == 2){
            err = chdir(pCmdLine->arguments[1]);
        }

        if(err)
            fprintf(stderr, "ERROR: Unknown direcrtory\n");
    }

    // history function
    else if(strcmp(pCmdLine->arguments[0], "history") == 0){
        print_history();
    }

    // general function - non shell
    else 
    {
        if ((pid = fork()) == -1)
            fprintf(stderr,"ERROR: fork error\n");

        else if(pid == 0){ // code executed by child


            // execute
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);

            // if execvp return, it failed
            printf("ERROR: %s command not found\n", pCmdLine->arguments[0]);
            
            // free all memory and exit
            free_history();
            freeCmdLines(pCmdLine);
            _exit(0);
        }
        else if(pCmdLine->blocking){ // code executed by parent
            waitpid(pid, NULL, 0);
        } 
    }
}

int main(int argc, char **argv) {

    char cwd[PATH_MAX];
    
    while(TRUE){

        // print current working directory 
        getcwd(cwd, PATH_MAX);
        printf("MyShell~%s$ ", cwd);

        // read command from user
        char input[MAX_READ];
        fgets(input, MAX_READ, stdin);
        input[strcspn(input, "\n")] = 0; // Removing trailing newline

        // quit or execute
        if(strcmp(input, "quit") == 0){
            printf("exiting...\n");
            // free history memory
            free_history();
            break;
        }
        
        if(strlen(input)==0)
            continue;

        cmdLine *cmd = parseCmdLines(input);
        
        execute(cmd);
        add_history(input);

        freeCmdLines(cmd);
    }

    return 0;
}