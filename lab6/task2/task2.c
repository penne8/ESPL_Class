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

pid_t pid;
char* h_array[HISTORY_SIZE];     // history list of commands
int h_count = 0;                // number of total commands
int h_pointer = 0;              // current command index in history list

void free_history(){
    for (int i = 0; i < HISTORY_SIZE; i++)
        free(h_array[i]);
}

void print_history(){
    int h_curr_pointer = h_pointer + 2;             // current command pointer inside the h_array
    int h_command_index = h_count - HISTORY_SIZE;   // current command index in shell

    // check edge case for h_array not full
    if(h_count < HISTORY_SIZE){
        h_curr_pointer = 0;
        h_command_index = 0;
    }

    /*
    continue untill curr h_position.
    (over all, HISTORY_SIZE-2 commands will be printed, all commands
    in h_array, but without the calling history command itself and
    the oldest command that will be deleted when the new command will be called.
    */
    while(h_command_index != h_count-1){    // -1 to fit count size to index
        if(h_curr_pointer == HISTORY_SIZE)
            h_curr_pointer = 0;
        
        cmdLine* pCmdLine = parseCmdLines(h_array[h_curr_pointer]);

        printf("%d)", h_command_index);
        for(int j=0; j<pCmdLine->argCount; j++)
            printf(" %s", pCmdLine->arguments[j]);
        printf("\n");

        freeCmdLines(pCmdLine);
        h_curr_pointer++;
        h_command_index++;
    }
}

void execute(char* sCmdLine){

    cmdLine* pCmdLine = parseCmdLines(sCmdLine);

    // check if first char of first argument is '!'
    if(pCmdLine->arguments[0][0] == 33){
        int restored_command_index = atoi(pCmdLine->arguments[0]+sizeof(char));
        if(restored_command_index >= h_count || restored_command_index < h_count - HISTORY_SIZE)
            fprintf(stderr,"ERROR: history command number '%d' not found. use 'history' command for valid options.\n", restored_command_index);
        else{
            int real_array_index = restored_command_index % HISTORY_SIZE;
            // check edge case for h_array not full
            if(h_count < HISTORY_SIZE)
                real_array_index = restored_command_index;

            printf("real_array_index:       %d ---------\n", real_array_index);
            printf("restored_command_index: %d ---------\n", restored_command_index);
            printf("h_count:                %d ---------\n", h_count);

            execute(h_array[real_array_index]);
        }
    }

    // cd function
    else if(strcmp(pCmdLine->arguments[0], "cd") == 0){
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
    else{

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

    freeCmdLines(pCmdLine);
}

int main(int argc, char **argv) {

    while(TRUE){
        
        // print current working directory 
        char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        printf("MyShell~%s$ ", cwd);

        // get user command and save in history array
        h_array[h_pointer] = (char *) malloc(MAX_READ);
        fgets(h_array[h_pointer], MAX_READ, stdin);

        // quit or execute
        if(strcmp(h_array[h_pointer], "quit\n") == 0){
            printf("exiting...\n");
            // free history memory
            free_history();
            break;
        }
        else{
            h_count++;

            execute(h_array[h_pointer]);

            // increment pointer, if reached to the end, start from begining
            h_pointer++;
            if(h_pointer == HISTORY_SIZE)
                h_pointer = 0;
        }

    }
    return 0;
}