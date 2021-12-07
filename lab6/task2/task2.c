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

#define FREE(X) if(X) free((void*)X)

pid_t pid;
char *h_array[HISTORY_SIZE];     // history list of commands
int h_count = 0;                // number of total commands
int h_pointer = 0;              // current command index in history list

void free_history() {
    for (int i = 0; i < HISTORY_SIZE; i++)
        FREE(h_array[i]);
}

void add_history(char *command) {
    FREE(h_array[h_pointer]);
    h_array[h_pointer] = (char *) calloc(MAX_READ, 1);
    strcpy(h_array[h_pointer], command);
    h_pointer++;
    h_pointer %= HISTORY_SIZE;
    h_count++;
}

void print_history() {
    int curr = h_pointer;
    int count = (h_count < HISTORY_SIZE) ? 0 : (h_count - HISTORY_SIZE);
    do {
        if (h_array[curr]) {
            printf("%d) %s\n", count, h_array[curr]);
            count++;
        } else if (h_count >= HISTORY_SIZE)
            count++;
        curr++;
        curr %= HISTORY_SIZE;
    } while (curr != h_pointer);
}

void execute(cmdLine *pCmdLine) {

    // check if first char of first argument is '!'
    if (pCmdLine->arguments[0][0] == 33) {
        int restored = atoi(pCmdLine->arguments[0] + sizeof(char));
        if (restored >= h_count || restored < h_count - HISTORY_SIZE) {
            fprintf(stderr, "ERROR: history command number '%d' ", restored);
            fprintf(stderr, "not found. use 'history' command for valid options.\n");
        } else {
            char *cmd_str = h_array[restored % HISTORY_SIZE];
            cmdLine *cmd = parseCmdLines(cmd_str);
            execute(cmd);
            add_history(cmd_str);
            freeCmdLines(cmd);

        }
        return;
    }

        // cd function
    else if (strcmp(pCmdLine->arguments[0], "cd") == 0) {
        char err = 1;

        if (pCmdLine->argCount == 1) {
            err = chdir(getenv("HOME"));
        } else if (pCmdLine->argCount == 2) {
            err = chdir(pCmdLine->arguments[1]);
        }

        if (err)
            fprintf(stderr, "ERROR: Unknown directory\n");
        return;
    }

        // history function
    else if (strcmp(pCmdLine->arguments[0], "history") == 0) {
        print_history();
        return;
    }

        // general function - non shell
    else {

        if ((pid = fork()) == -1)
            fprintf(stderr, "ERROR: fork error\n");

        else if (pid == 0) { // code executed by child


            // execute
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);

            // if execvp return, it failed
            printf("ERROR: %s command not found\n", pCmdLine->arguments[0]);

            // free all memory and exit
            free_history();
            freeCmdLines(pCmdLine);
            _exit(0);
        } else if (pCmdLine->blocking) { // code executed by parent
            waitpid(pid, NULL, 0);
        }
    }
}

int main(int argc, char **argv) {

    char cwd[PATH_MAX];
    while (TRUE) {

        // print current working directory
        getcwd(cwd, PATH_MAX);
        printf("MyShell~%s$ ", cwd);

        // get user command and save in history array
        char input[MAX_READ];
        fgets(input, MAX_READ, stdin);
        input[strcspn(input, "\n")] = 0; // Removing trailing newline

        // quit or execute
        if (strcmp(input, "quit") == 0) {
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