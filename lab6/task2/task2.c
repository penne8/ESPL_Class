#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define HISTORY_SIZE 5
#define MAX_READ 2048

pid_t pid;
cmdLine* h_list[HISTORY_SIZE+1] = {NULL};
int h_pointer = 0;
int max_history = -1;

void free_history() {
    for (int i = 0; i <= h_pointer; i++)
        freeCmdLines(h_list[i]);
}

void print_history() {
    int curr_h_pointer = h_pointer + 2;
    int curr_h_number = max_history - HISTORY_SIZE;
    if(curr_h_pointer == HISTORY_SIZE+1)
        curr_h_pointer = 0;
    else if(curr_h_pointer == HISTORY_SIZE+2)
        curr_h_pointer = 1;
    else if(h_list[curr_h_pointer] == NULL){ //loop did not happend, reset to 0.
        curr_h_pointer = 0;
        curr_h_number = 0;
    }

    while(h_list[curr_h_pointer] != NULL){
        printf("%d)", curr_h_number);
        for(int j=0; j<h_list[curr_h_pointer]->argCount; j++)
            printf(" %s", h_list[curr_h_pointer]->arguments[j]);
        printf("\n");
        curr_h_number++;
        curr_h_pointer++;
        if(curr_h_pointer == HISTORY_SIZE+1){
            curr_h_pointer = 0;
        }
    }
}

void execute(cmdLine *pCmdLine){

    // check if first char of first argument is '!'
    if(pCmdLine->arguments[0][0] == 33){
        int restored_command_index = atoi(pCmdLine->arguments[0]+sizeof(char));
        if(restored_command_index >= h_pointer || restored_command_index < 0)
            fprintf(stderr,"ERROR: history command number '%d' not found. use 'history' command for valid options.\n", restored_command_index);
        else
            execute(h_list[restored_command_index]);
    }

    // cd function
    else if(strcmp(pCmdLine->arguments[0], "cd") == 0){
        char err = 0;
        if(pCmdLine->argCount == 1){
            err = chdir(getenv("HOME"));
        }
        else{
            err = chdir(pCmdLine->arguments[1]);
        }

        if(err || pCmdLine->argCount > 2)
            fprintf(stderr, "ERROR: Unknown direcrtory\n");
    }

    // history function
    else if(strcmp(pCmdLine->arguments[0], "history") == 0){
        print_history();
    }

    // general function - non shell
    else{
        if ((pid = fork()) == -1)
            fprintf(stderr,"ERROR: fork error\n");
        else if(pid == 0){ // code executed by child
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);
            // if execvp return, it failed
            printf("ERROR: %s command not found\n", pCmdLine->arguments[0]);
            _exit(0);
        }
        else if(pCmdLine->blocking){ // code executed by parent
            waitpid(pid, NULL, 0);
        } 
    }
}

int main(int argc, char **argv) {
    char cwd[PATH_MAX];

    while(1){
        getcwd(cwd, PATH_MAX);
        printf("MyShell~%s$ ", cwd);

        char sCmdLine[MAX_READ];
        fgets(sCmdLine, MAX_READ, stdin);

        // parse into history command
        h_list[h_pointer] = parseCmdLines(sCmdLine);
        max_history++;

        if(strcmp(h_list[h_pointer]->arguments[0], "quit") == 0){
            printf("exiting...\n");
            free_history();
            break;
        }
        else{
            execute(h_list[h_pointer]);
        }

        // edge case for reaching max history size
        if (h_pointer == HISTORY_SIZE){
            freeCmdLines(h_list[0]);
            h_list[0] = NULL;
            h_pointer = 0;
        }
        else
            h_pointer++;
        
        
    }
    return 0;
}