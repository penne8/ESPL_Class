#include <sys/wait.h> /* Needed to use wait() */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* UNIX and POSIX constants and functions (fork, pipe) */
#include <string.h> /* Needed to use strlen() */

int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf[32];

    if (pipe(pipefd) == -1) /* An error has occurred. */
    {
        fprintf(stderr, "%s", "The call to pipe() has failed.\n");
        exit(EXIT_FAILURE);
    }
    if ((cpid = fork()) == -1) /* An error has occurred. */
    {
        fprintf(stderr, "%s", "The call to fork() has failed.\n");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0)
    { /* Child */
        close(pipefd[0]); /* Close unused read end */
        write(pipefd[1], "hello", strlen("hello"));
        close(pipefd[1]); /* Closing creates the EOF marker. */

        _exit(EXIT_SUCCESS);
    }
    else
    { /* Parent */
        close(pipefd[1]); /* Close unused write end */
        wait(NULL); /* Parent waits for the child to terminate */
        read(pipefd[0], buf, 32);
        printf("%s\n", buf);
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }
    return 0;
}