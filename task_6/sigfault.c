#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
char c;
int fd1, fd2, i = 128;
pid_t pid;
int bits = 8;
void sigfault(int signum){
  if (pid == 0){ //child handler
    if (signum == SIGUSR1){
      if(bits == 8){
        bits = 0;
        i = 128;
        if((read(fd1, &c, 1)) == 0){
            close(fd1);
            exit(0);
        }
      }
      bits++;
      if(c&i){
        i /= 2;
        kill(getppid(), SIGUSR1);
      }
      else{
        i /= 2;
        kill(getppid(), SIGUSR2);
      }
    }
  }
  else{ //parent handler
    if(bits == 8){
      write(fd2, &c, 1);
      bits = 0;
      c = 0;
    }
    if(signum == SIGUSR1){
      bits++;
      c = 2*c + 1;
      kill(pid, SIGUSR1);
    }
    if(signum == SIGUSR2){
      bits++;
      c = 2*c;
      kill(pid, SIGUSR1);
    }
  }
}
void sigc(int signum){
  write(fd2, &c, 1);
  close(fd2);
  exit(0);
}
int main(int argc, char *argv[]){
  struct sigaction act1;
  struct sigaction act2;
  act1.sa_handler = sigfault;
  sigaction(SIGUSR1, &act1, NULL);
  sigaction(SIGUSR2, &act1, NULL);
  act2.sa_handler = sigc;
  sigaction(SIGCHLD, &act2, NULL);
  fd1 = open(argv[1], O_RDONLY);
  fd2 = open(argv[2], O_CREAT|O_TRUNC|O_WRONLY, 0777);
  pid = fork();
  if(pid > 0){ //parent
    bits = 0;
    kill(pid, SIGUSR1);
    while(1);
  }
  if(pid == 0){ //child
    while(1);
    }
  else{
    perror("Fork error");
    exit(-1);
  }
  return 0;
}
