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
int handle_command(cmdLine *pCmdLine);

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
    int cmd_count = 0;
    cmdLine *c = pCmdLine;
    while (c != NULL)
    {
        cmd_count++;
        c = c->next;
    }
    int nPipes = cmd_count - 1;
    int **pipes = createPipes(nPipes);
    // pid_t pid_c1;
    // pid_t pid_c2;

    // for each pipe 2 children pid
    pid_t **children_pid = (pid_t **)malloc(sizeof(pid_t **) * nPipes);
    for (int i = 0; i < nPipes; i++)
        children_pid[i] = (pid_t *)malloc(sizeof(pid_t *) * 2);

    cmdLine *curr_cmd = pCmdLine;

    for (int i = 0; i < nPipes; i++)
    {
        fprintf(stderr,"pipe:%d\n",i);
        int *curr_pipe = pipes[i]; // pipe between curr cmd and next cmd
        if (pipe(curr_pipe) == -1) // open pipe
        {
            fprintf(stderr, "%s", "The call to pipe() has failed.\n");
            exit(EXIT_FAILURE);
        }
        // Fork child 1
        switch (children_pid[i][0] = fork()) //output child
        {
        case -1:
            fprintf(stderr, "ERROR: fork error\n");
            exit(EXIT_FAILURE);
            break;
        case 0: // code executed by child 1
            if (leftPipe(pipes, curr_cmd))
            {
                dup2(curr_pipe[1], leftPipe(pipes, curr_cmd)[1]); // steal output from previous pipe
                fprintf(stderr, "There is a left pipe\n");
            }
            else
            {
                dup2(curr_pipe[1], 1); // steal output from stdout
                fprintf(stderr, "replacing stdout!\n");
            }
            close(curr_pipe[1]);
            redirect_io(curr_cmd);
            execvp(curr_cmd->arguments[0], curr_cmd->arguments);

            free_mem(pCmdLine);
            _exit(EXIT_SUCCESS);
            break;
        default:
            break;
        }
        // Close write-end
        close(curr_pipe[1]);

        // Fork child 2
        switch (children_pid[i][1] = fork()) // reading child
        {
        case -1:
            fprintf(stderr, "ERROR: fork error\n");
            exit(EXIT_FAILURE);
            break;
        case 0: // code executed by child 1
            printf("BOOM");
            if (leftPipe(pipes, curr_cmd))
            {
                dup2(curr_pipe[0], leftPipe(pipes, curr_cmd)[0]); // steal input from previous pipe
                fprintf(stderr, "There is a left pipe\n");
            }
            else
            {
                dup2(curr_pipe[0], 0);
                fprintf(stderr, "replacing stdin!\n"); // steal input from stdin
            }
            close(curr_pipe[0]);
            redirect_io(curr_cmd->next);
            execvp(curr_cmd->next->arguments[0], curr_cmd->next->arguments);

            free_mem(pCmdLine);
            _exit(EXIT_SUCCESS);
            break;
        default:
            break;
        }

        // Close read-end file
        close(curr_pipe[0]);

        // // Waiting
        // waitpid(pid_c1, NULL, 0);
        // waitpid(pid_c2, NULL, 0);

        curr_cmd = curr_cmd->next;
    }
    fprintf(stderr, "finished loop!\n");
    for (int child = 0; child < nPipes; child++)
    {
        waitpid(children_pid[child][1], NULL, 0);
        waitpid(children_pid[child][0], NULL, 0);
    }
    for (int i = 0; i < nPipes; i++)
        free(children_pid[i]);
    free(children_pid);
    releasePipes(pipes, nPipes);
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
        return print_history();

    // general function - non shell
    return execute(pCmdLine);
}

int main(int argc, char **argv)
{
    char cwd[PATH_MAX];
    while (TRUE)
    {
        // print current working directory
        getcwd(cwd, PATH_MAX);
        printf("MyShell~%s$ ", cwd);
        // read command from user
        char input[MAX_READ];
        fgets(input, MAX_READ, stdin);
        input[strcspn(input, "\n")] = 0; // Removing trailing newline
        // quit or handle command
        if (strcmp(input, "quit") == 0)
        {
            printf("exiting...\n");
            free_history();
            break;
        }
        if (strlen(input) == 0)
            continue;
        mainCmd = parseCmdLines(input);
        int add = handle_command(mainCmd);
        if (add) // add to history if needed
            add_history(input);
        freeCmdLines(mainCmd); // free current cmdLine
    }
    return 0;
}