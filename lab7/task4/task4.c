#include <stdlib.h>
#include "LineParser.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define HISTORY_SIZE 5
#define MAX_READ 2048
#define TRUE 1
#define ADD_HISTORY 1
#define DONT_ADD_HISTORY 0

extern int **createPipes(int nPipes);                  // lab7-test4
extern void releasePipes(int **pipes, int nPipes);     // lab7-test4
extern int *leftPipe(int **pipes, cmdLine *pCmdLine);  // lab7-test4
extern int *rightPipe(int **pipes, cmdLine *pCmdLine); // lab7-test4

int handle_command(cmdLine *pCmdLine); // lab 6

int pipefd[2];
pid_t pid;
cmdLine *mainCmd;
char *h_array[HISTORY_SIZE]; // History list of commands
int h_count = 0;             // Number of total commands
int h_pointer = 0;           // Current command index in history list

void redirect_io(cmdLine *pCmdLine) //lab7 - task1
{
    int fd;
    if (pCmdLine->inputRedirect)
    {
        fd = open(pCmdLine->inputRedirect, O_RDONLY, S_IRUSR);
        dup2(fd, 0); //replace stdin with redirect
    }
    if (pCmdLine->outputRedirect)
    {
        fd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR);
        dup2(fd, 1); //replace stdout with redirect
    }
}

void free_history()
{
    for (int i = 0; i < HISTORY_SIZE; i++)
        free(h_array[i]);
}

void free_mem(cmdLine *pCmdLine)
{
    free_history();
    if (pCmdLine)
        freeCmdLines(pCmdLine);
    if (mainCmd != pCmdLine)
        freeCmdLines(mainCmd);
}

void add_history(char *cmd_str)
{
    free(h_array[h_pointer]);
    h_array[h_pointer] = (char *)malloc(MAX_READ);
    strcpy(h_array[h_pointer], cmd_str);

    h_pointer++;
    h_pointer %= HISTORY_SIZE;
    h_count++;
}

int exclamation_mark_case(cmdLine *pCmdLine)
{
    int restored_command_index = atoi(pCmdLine->arguments[0] + sizeof(char));

    if (restored_command_index == 0 && strcmp(pCmdLine->arguments[0], "!0") != 0)
    {
        fprintf(stderr, "ERROR: didn't recognized valid number as input. try again.\n");
        return ADD_HISTORY;
    }

    if (restored_command_index >= h_count || restored_command_index < h_count - HISTORY_SIZE)
    {
        fprintf(stderr, "ERROR: history command number '%d' not found. Use 'history' command for valid options.\n", restored_command_index);
        return ADD_HISTORY;
    }

    int real_array_index = restored_command_index % HISTORY_SIZE;
    char *cmd_str = h_array[real_array_index];

    cmdLine *cmd = parseCmdLines(cmd_str);
    handle_command(cmd);
    freeCmdLines(cmd);

    /*  For edge case when '!X' command will enter the same
        entry in h_array as the called command, omit adding. */
    if (real_array_index != h_pointer)
    {
        /*  Add the true function that was intended and not
            the '!X' command.
            That way, if you call a restored function that
            not in the array anymore, it won't fail. */
        add_history(cmd_str);
    }
    else
    {
        h_pointer++;
        h_pointer %= HISTORY_SIZE;
        h_count++;
    }

    // don't add the '!X' command
    return DONT_ADD_HISTORY;
}

int cd_case(cmdLine *pCmdLine)
{
    char err = 1;

    if (pCmdLine->argCount == 1)
    {
        err = chdir(getenv("HOME"));
    }
    else if (pCmdLine->argCount == 2)
    {
        err = chdir(pCmdLine->arguments[1]);
    }

    if (err)
        fprintf(stderr, "ERROR: Unknown direcrtory\n");

    return ADD_HISTORY;
}

int print_history()
{
    // Current command pointer inside h_array
    int h_curr_pointer = (h_count < HISTORY_SIZE) ? 0 : ((h_pointer + 1) % HISTORY_SIZE);
    // Current command index in shell
    int h_command_index = (h_count < HISTORY_SIZE) ? 0 : (h_count - HISTORY_SIZE + 1);

    /*
    Continue untill curr h_position.
    [over all, HISTORY_SIZE-1 commands will be printed: all commands
    in h_array besides the oldest command that the calling history
    command will replace in the h_array.
    (It won't be relevent to call for in the next command)]
    */
    while (h_command_index != h_count)
    {

        if (h_array[h_curr_pointer])
        {
            printf("%d) %s\n", h_command_index, h_array[h_curr_pointer]);
        }

        h_curr_pointer++;
        h_curr_pointer %= HISTORY_SIZE;
        h_command_index++;
    }

    return ADD_HISTORY;
}

int execute(cmdLine *pCmdLine)
{
    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "ERROR: fork error\n");
        break;
    case 0: // code executed by child
        redirect_io(pCmdLine);
        // execute
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);

        // if execvp return, it failed
        printf("ERROR: %s command not found\n", pCmdLine->arguments[0]);

        // free all memory and exit
        free_mem(pCmdLine);
        _exit(0);
        break;
    default:
        if (pCmdLine->blocking) // code executed by parent
            waitpid(pid, NULL, 0);
        break;
    }

    return ADD_HISTORY;
}

int handle_pipe_command(cmdLine *pCmdLine)
{
    pid_t pid_c1;
    pid_t pid_c2;

    if (pipe(pipefd) == -1)
    {
        fprintf(stderr, "%s", "The call to pipe() has failed.\n");
        exit(EXIT_FAILURE);
    }

    // Fork child 1
    switch (pid_c1 = fork())
    {
    case -1:
        fprintf(stderr, "ERROR: fork error\n");
        exit(EXIT_FAILURE);
        break;
    case 0: // code executed by child 1
        close(STDOUT_FILENO);
        dup(pipefd[1]);
        close(pipefd[1]);

        redirect_io(pCmdLine);
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);

        free_mem(pCmdLine);
        _exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }

    // Close write-end
    close(pipefd[1]);

    // Fork child 2
    switch (pid_c2 = fork())
    {
    case -1:
        fprintf(stderr, "ERROR: fork error\n");
        exit(EXIT_FAILURE);
        break;
    case 0: // code executed by child 1
        close(STDIN_FILENO);
        dup(pipefd[0]);
        close(pipefd[0]);
        redirect_io(pCmdLine->next);
        execvp(pCmdLine->next->arguments[0], pCmdLine->next->arguments);

        free_mem(pCmdLine);
        _exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }

    // Close read-end file
    close(pipefd[0]);

    // Waiting
    waitpid(pid_c1, NULL, 0);
    waitpid(pid_c2, NULL, 0);

    return ADD_HISTORY;
}

int handle_command(cmdLine *pCmdLine)
{
    // check for pipe case
    if (pCmdLine->next != NULL)
        return handle_pipe_command(pCmdLine);

    // check if first char of first argument is '!'
    if (pCmdLine->arguments[0][0] == 33)
        return exclamation_mark_case(pCmdLine);

    // cd function
    if (strcmp(pCmdLine->arguments[0], "cd") == 0)
        return cd_case(pCmdLine);

    // history function
    if (strcmp(pCmdLine->arguments[0], "history") == 0)
    {
        return print_history();
    }

    // general function - non shell
    return execute(pCmdLine);
}

int main(int argc, char **argv)
{
    char *string = "ls | tee | cat | more";
    cmdLine *cmd = parseCmdLines(string);
    int **pipes = createPipes(3);
    if (leftPipe(pipes, cmd) == NULL && rightPipe(pipes, cmd->next->next->next) == NULL)
        printf("pass\n");
    else
        printf("fail\n");
    printf("1. %d\n", leftPipe(pipes, cmd->next)[0]);
    printf("2. %d\n", leftPipe(pipes, cmd->next)[1]);
    int* fd = rightPipe(pipes, cmd->next);
    printf("3. %d\n", fd[0]);
    printf("4. %d\n", fd[1]);
    freeCmdLines(cmd);
    releasePipes(pipes, 3);

    // char cwd[PATH_MAX];

    // while (0)
    // {

    //     // print current working directory
    //     getcwd(cwd, PATH_MAX);
    //     printf("MyShell~%s$ ", cwd);

    //     // read command from user
    //     char input[MAX_READ];
    //     fgets(input, MAX_READ, stdin);
    //     input[strcspn(input, "\n")] = 0; // Removing trailing newline

    //     // quit or handle command
    //     if (strcmp(input, "quit") == 0)
    //     {
    //         printf("exiting...\n");

    //         // free memory
    //         free_history();
    //         break;
    //     }

    //     if (strlen(input) == 0)
    //         continue;

    //     mainCmd = parseCmdLines(input);

    //     int add = handle_command(mainCmd);

    //     // add to history if needed
    //     if (add)
    //         add_history(input);

    //     // free current cmdLine
    //     freeCmdLines(mainCmd);
    // }

    return 0;
}