#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct dpipe{
  int tdx[2];
  int rdx[2];
};
typedef struct dpipe dpipe_t;
int main(){
  char bufp[4096];
  char bufc[4096];
  dpipe_t dup;
  int pid, ppid, sizep, sizec, writtenp, writtenc;
  if (pipe(dup.tdx)<0){
    printf("Pipe error\n");
    return -1;
  }
  if (pipe(dup.rdx)<0){
    printf("Pipe error\n");
    return -1;
  }
  printf("Pipe initialized. Type \"quit\" to exit\n");
  pid = fork();
  if (pid < 0){
    printf("Fork error\n");
    close(dup.tdx[0]);
    close(dup.tdx[1]);
    close(dup.rdx[0]);
    close(dup.rdx[1]);
    return -1;
  }
  else if (pid == 0) {
    ppid = getppid();
    close(dup.tdx[1]);
    close(dup.rdx[0]);
    while(1){
      sizec = read(dup.tdx[0], bufc, sizeof(bufc));
      if (sizec < 0){
        printf("Reading error\n");
        close(dup.tdx[0]);
        close(dup.rdx[1]);
        exit(-1);
      }
      if (sizec == 0){
        close(dup.tdx[0]);
        close(dup.rdx[1]);
        exit(0);
      }
      write(1, "Received ", 9);
      if (write(1, bufc, sizec) < 0){
        printf("Writing error\n");
        close(dup.tdx[0]);
        close(dup.rdx[1]);
        exit(-1);
      }
      write(1, "Sending ", 8);
      sizec = read(0, bufc, sizeof(bufc));
      if (sizec < 0){
        printf("Reading error\n");
        close(dup.rdx[1]);
        close(dup.tdx[0]);
        exit(-1);
      }
      if (!strncmp(bufc, "quit\n", 5)){
        close(dup.rdx[1]);
        close(dup.tdx[0]);
        exit(0);
      }
      if (write(dup.rdx[1], bufc, sizec) < 0){
        printf("Writing error\n");
        close(dup.rdx[1]);
        close(dup.tdx[0]);
        exit(-1);
      }
    }
  }
  else {
    close(dup.rdx[1]);
    close(dup.tdx[0]);
    while(1){
      write(1, "Sending ", 8);
      sizep = read(0, bufp, sizeof(bufp));
      if (sizep < 0){
        printf("Reading error\n");
        close(dup.rdx[0]);
        close(dup.tdx[1]);
        exit(-1);
      }
      if (!strncmp(bufp, "quit\n", 5)){
        close(dup.rdx[0]);
        close(dup.tdx[1]);
        exit(0);
      }
      if (write(dup.tdx[1], bufp, sizep) < 0){
        printf("Writing error\n");
        close(dup.rdx[0]);
        close(dup.tdx[1]);
        exit(-1);
      }
      sizep = read(dup.rdx[0], bufp, sizeof(bufp));
      if (sizep < 0){
        printf("Reading error\n");
        close(dup.rdx[0]);
        close(dup.tdx[1]);
        exit(-1);
      }
      if (sizep == 0){
        close(dup.rdx[0]);
        close(dup.tdx[1]);
        exit(0);
      }
      write(1, "Received ", 9);
      if (write(1, bufp, sizep) < 0){
        printf("Writing error\n");
        close(dup.rdx[0]);
        close(dup.tdx[1]);
        exit(-1);
      }
    }
  }
  return 0;
}
