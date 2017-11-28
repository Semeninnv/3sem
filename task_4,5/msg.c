#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
int main(int argc, char *argv[]){
  umask(0);
  struct msgbuf{
    long type;
    char buf[4096];
    int length;
  } mbuf;
  key_t key = ftok("./msg.c", 0);
  int msqid = msgget(key, 0600 | IPC_CREAT);
  pid_t pid = fork();
  if (pid > 0){
    mbuf.type = 1;
    int fd1 = open(argv[1], O_RDONLY);
    while ((mbuf.length = read(fd1, mbuf.buf, sizeof(mbuf.buf))) > 0){
      msgsnd(msqid, &mbuf, sizeof(mbuf) - sizeof(long), 0);
    }
    mbuf.type = 2;
    msgsnd(msqid, &mbuf, 0, 0);
    close(fd1);
  }
  if(pid == 0){
    int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0700);
    while(1){
      msgrcv(msqid, &mbuf, sizeof(mbuf) - sizeof(long), 0, 0);
      if(mbuf.type == 2)
        break;
      write(fd2, mbuf.buf, mbuf.length);
    }
    msgctl(msqid, IPC_RMID, NULL);
    close(fd2);
  }
  return 0;
}
