#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define HISTORY_SIZE 1
#define MAX_READ 2048

pid_t pid;
cmdLine** h_list = NULL;
int h_pointer = 0;

void freeHistory() {
    for (int i = 0; i <= h_pointer; i++)
        freeCmdLines(h_list[i]);
    h_pointer = 0;
}

void execute(cmdLine *pCmdLine){

    // check if first char of first argument is '!'
    if(pCmdLine->arguments[0][0] == 33){
        int restored_command_index = atoi(pCmdLine->arguments[0]+sizeof(char));
        if(restored_command_index >= h_pointer || restored_command_index < 0)
            fprintf(stderr,"ERROR: history non existent. use 'history' command for valid options.\n");
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
            fprintf(stderr, "ERROR: Unknown direcrtory");
    }

    // history function
    else if(strcmp(pCmdLine->arguments[0], "history") == 0){
        for(int i=0; i<h_pointer; i++){
            printf("%d) %s\n", i, h_list[i]->arguments[0]);
        }
    }

    // general function - non shell
    else{
        if ((pid = fork()) == -1)
            fprintf(stderr,"ERROR: fork error\n");
        else if(pid == 0){ // code executed by child
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);
            // if execvp return, it failed
            printf("Unkown command\n");
            _exit(0);
        }
        else if(pCmdLine->blocking){ // code executed by parent
            waitpid(pid, NULL, 0);
        } 
    }
}

int main(int argc, char **argv) {
    h_list = (cmdLine **) malloc(sizeof(cmdLine *)*HISTORY_SIZE);
    char cwd[PATH_MAX];

    int should_exit = 0;
    while(!should_exit){

        getcwd(cwd, PATH_MAX);
        printf("MyShell~%s$ ", cwd);

        char sCmdLine[MAX_READ];
        fgets(sCmdLine, MAX_READ, stdin);

        // edge case for reaching max history size
        if (h_pointer >= HISTORY_SIZE)
            freeHistory();
        // parse into history command
        h_list[h_pointer] = parseCmdLines(sCmdLine);

        if(strcmp(h_list[h_pointer]->arguments[0], "quit") == 0){
            printf("exiting...\n");
            freeHistory();
            free(h_list);
            should_exit = 1;
        }
        else{
            execute(h_list[h_pointer]);
        }

        h_pointer++;
        
    }
    return 0;
}