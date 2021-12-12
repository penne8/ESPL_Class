#include <stdlib.h>
#include "LineParser.h"
#include "task4.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

// ---  global variables ----
cmdLine *mainCmd;            // lab6 - save for memory release
char *h_array[HISTORY_SIZE]; // lab6 - History list of commands
int h_count = 0;             // lab6 - Number of total commands
int h_pointer = 0;           // lab6 - Current command index in history list
// -----------------------

int main(int argc, char **argv) // lab6
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

int handle_command(cmdLine *pCmdLine) // lab7
{
    // check for pipe case
    if (pCmdLine->next != NULL)
        return handle_pipe_command(pCmdLine); // lab7

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

int handle_pipe_command(cmdLine *pCmdLine) // lab7
{
    int cmd_count = getCmdCount(pCmdLine);
    int nPipes = cmd_count - 1;
    int **pipes = createPipes(nPipes);
    pid_t *children_pid = (pid_t *)malloc(sizeof(pid_t **) * cmd_count); // each command have child process
    cmdLine *curr_cmd = pCmdLine;
    openPipes(pipes, nPipes);

    for (int cmd_ind = 0; cmd_ind < cmd_count; cmd_ind++) // loop over commands
    {
        int *lpipe = leftPipe(pipes, curr_cmd);
        int *rpipe = rightPipe(pipes, curr_cmd);
        switch (children_pid[cmd_ind] = fork())
        {
        case -1:
            fprintf(stderr, "ERROR: fork error\n");
            free(children_pid);
            releasePipes(pipes, nPipes);
            exit(EXIT_FAILURE);
        case 0:
            if (lpipe) // if there is a left pipe get input from him
            {
                dup2(lpipe[0], 0);
                close(lpipe[0]);
            }
            if (rpipe) // if there is a right pipe outout to him
            {
                dup2(rpipe[1], 1);
                close(rpipe[1]);
            }
            redirect_io(curr_cmd); //support <>
            execvp(curr_cmd->arguments[0], curr_cmd->arguments);
            free(children_pid);
            releasePipes(pipes, nPipes);
            free_mem(pCmdLine);
            _exit(EXIT_SUCCESS);
        }
        if (rpipe)
            close(rpipe[1]);
        if (lpipe)
            close(lpipe[0]);
        curr_cmd = curr_cmd->next;
    }
    for (int child = 0; child < cmd_count; child++) // wait for all children processors by their order
        waitpid(children_pid[child], NULL, 0);
    free(children_pid);
    releasePipes(pipes, nPipes);
    return ADD_HISTORY;
}

int getCmdCount(cmdLine *pCmdLine) // lab7
{
    cmdLine *c = pCmdLine;
    int cmd_count = 0;
    while (c != NULL)
    {
        cmd_count++;
        c = c->next;
    }
    return cmd_count;
}

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

int execute(cmdLine *pCmdLine) // lab6
{
    int pid;
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

void free_history() // lab6
{
    for (int i = 0; i < HISTORY_SIZE; i++)
        free(h_array[i]);
}

void free_mem(cmdLine *pCmdLine) // lab6
{
    free_history();
    if (pCmdLine)
        freeCmdLines(pCmdLine);
    if (mainCmd != pCmdLine)
        freeCmdLines(mainCmd);
}

void add_history(char *cmd_str) // lab6
{
    free(h_array[h_pointer]);
    h_array[h_pointer] = (char *)malloc(MAX_READ);
    strcpy(h_array[h_pointer], cmd_str);
    h_pointer++;
    h_pointer %= HISTORY_SIZE;
    h_count++;
}

int exclamation_mark_case(cmdLine *pCmdLine) // lab6
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

int cd_case(cmdLine *pCmdLine) // lab6
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

int print_history() // lab6
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
