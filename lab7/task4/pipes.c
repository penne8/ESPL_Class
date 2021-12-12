#include "LineParser.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

// This function receives the number of required pipes and returns an array of pipes.
int **createPipes(int nPipes)
{
    int **pipes = (int **)malloc((nPipes + 1) * sizeof(int *)); //one more for null to determine end of array
    pipes[nPipes] = NULL;
    for (int i = 0; i < nPipes; i++)
    {
        pipes[i] = (int *)malloc(sizeof(int) * 2);
        pipe(pipes[i]);
    }
    return pipes;
}

// This function receives an array of pipes and an integer indicating the size of the array.
// The function releases all memory dedicated to the pipes.
void releasePipes(int **pipes, int nPipes)
{
    for (int i = 0; i < nPipes; i++)
        free(pipes[i]);
    free(pipes);
}

//This function receives an array of pipes and a pointer to a cmdLine structure.
//It returns the pipe which feeds the process associated with the command.
//That is, the pipe that appears to the left of the process name in the command line.
// For example, the left pipe of process tee in pipeline "cat | tee | more" is the first pipe.
// If the command does not have a left pipe (as with cat in our example), the function returns NULL.
int *leftPipe(int **pipes, cmdLine *pCmdLine)
{
    int nPipes = 0;
    for (int i = 0; pipes[i] != NULL; i++)
        nPipes++;
    int ind = pCmdLine->idx - 1;
    if (ind >= 0 && ind < nPipes)
        return pipes[ind];
    return NULL;
}

//This function receives an array of pipes and a pointer to a cmdLine structure.
//It returns the pipe which is the sink of the associated command.
//That is, the pipe that appears to the right of the process name in the command line.
//For example, the right pipe of process tee in pipeline "cat | tee | more" is the second pipe.
//If the command does not have a right pipe (as with more in our example), the function returns NULL.
int *rightPipe(int **pipes, cmdLine *pCmdLine)
{
    int nPipes = 0;
    for (int i = 0; pipes[i] != NULL; i++)
        nPipes++;
    int ind = pCmdLine->idx;
    if (ind >= 0 && ind < nPipes)
        return pipes[ind];
    return NULL;
}