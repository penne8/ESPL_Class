#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define HISTORY_SIZE 5
#define MAX_READ 2048
#define TRUE 1

pid_t pid;
cmdLine* h_list[HISTORY_SIZE];  // history list of commands
int h_count = 0;                // number of total commands
int h_pointer = 0;              // current command index in history list  

void free_history(){
    for (int i = 0; i < HISTORY_SIZE; i++)
        freeCmdLines(h_list[i]);
}

void print_history(){
    int h_curr_pointer = h_pointer+1;               // current command pointer inside the h_list
    int h_command_index = h_count - HISTORY_SIZE;   // current command index in shell

    // check edge case for h_list not full
    if(h_command_index < 0){
        h_curr_pointer = 0;
        h_command_index = 0;
    }

    /*
    continue untill curr h_position.
    (over all, HISTORY_SIZE-1 commands will be printed, all commands
    in h_list, but without the calling history command itself.
    */
    while(h_command_index != h_count-1){    // -1 for fit count size to index
        if(h_curr_pointer == HISTORY_SIZE)
            h_curr_pointer = 0;

        printf("%d)", h_command_index);
        for(int j=0; j<h_list[h_curr_pointer]->argCount; j++)
            printf(" %s", h_list[h_curr_pointer]->arguments[j]);
        printf("\n");

        h_curr_pointer++;
        h_command_index++;
    }
}

void execute(cmdLine *pCmdLine){

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
    else{

        if ((pid = fork()) == -1)
            fprintf(stderr,"ERROR: fork error\n");

        else if(pid == 0){ // code executed by child

            // execute
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

    while(TRUE){
        
        // print current working directory 
        char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        printf("MyShell~%s$ ", cwd);

        // get user command
        char sCmdLine[MAX_READ];
        fgets(sCmdLine, MAX_READ, stdin);

        // free memory and replace with new one, increment history count
        freeCmdLines(h_list[h_pointer]);
        h_list[h_pointer] = parseCmdLines(sCmdLine);
        h_count++;

        // quit or execute
        if(strcmp(h_list[h_pointer]->arguments[0], "quit") == 0){
            printf("exiting...\n");
            // free history memory
            free_history();
            break;
        }
        else{
            execute(h_list[h_pointer]);
        }

        // increment pointer, if reached to the end, start from begining
        h_pointer++;
        if(h_pointer == HISTORY_SIZE)
            h_pointer = 0;
    }
    return 0;
}