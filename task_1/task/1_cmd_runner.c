/*
  Write a program which allow you to:
- run another programs via command line.
- get exit codes of terminated programs

## TIPS:
1. Use "2_fork_wait_exit.c" and "4_exec_dir.c" from examples. Combine them.
2. Parse input string according to the type of exec* (see "man exec").
   a) if execvp is used, string splitting into "path" and "args" is all you need.
3. Collect exit codes via waitpid/WEXITSTATUS.
*/
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 1000
int main() {
  int status, a, pid, i, i1, k;
  char *arg[N];
  char c;
  while(1){
    for(i = 0; i < N; i++)
      arg[i] = (char*) malloc(N);
    printf("Enter a file path with arguments. Type \"quit\" to exit\n");
    i = 0;
    k = 0;
    while(1){
      c = getchar();
      if(c == ' '){
        *(arg[i] + k) = 0;
        i++;
        k = 0;
      }
      else if(c == '\n'){
        *(arg[i] + k) = 0;
        i++;
        break;
      }
      else{
        *(arg[i] + k) = c;
        k++;
      }
    }
    if(!strcmp(arg[0], "quit"))
      return 0;
    free(arg[i]);
    i1 = i;
    arg[i1] = NULL;
    pid = fork();
    if (pid < 0){
        printf("Fork() error\n");
    }
    else if (pid == 0){
        a = execvp(arg[0], arg);
        exit(a);
    }
    else {
        waitpid(pid, &status, 0);
        printf("Process %d terminated. Return code: %d\n", pid, WEXITSTATUS(status));
    }
    arg[i1] = (char*) malloc(N);
  }
  return 0;
}
