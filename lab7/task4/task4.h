#define HISTORY_SIZE 5
#define MAX_READ 2048
#define TRUE 1
#define ADD_HISTORY 1
#define DONT_ADD_HISTORY 0

extern int **createPipes(int nPipes);                  // lab7- task4
extern void releasePipes(int **pipes, int nPipes);     // lab7- task4
extern int *leftPipe(int **pipes, cmdLine *pCmdLine);  // lab7- task4
extern int *rightPipe(int **pipes, cmdLine *pCmdLine); // lab7- task4
extern void openPipes(int **pipes, int nPipes);               // lab7- task4

int handle_command(cmdLine *pCmdLine);
void redirect_io(cmdLine *pCmdLine);
void free_history();
void free_mem(cmdLine *pCmdLine);
void add_history(char *cmd_str);
int exclamation_mark_case(cmdLine *pCmdLine);
int cd_case(cmdLine *pCmdLine);
int print_history();
int execute(cmdLine *pCmdLine);
int getCmdCount(cmdLine *pCmdLine);
int handle_pipe_command(cmdLine *pCmdLine);
int handle_command(cmdLine *pCmdLine);