#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
int main(int argc, char const *argv[]) {
  int fd1 = open(argv[1], O_RDWR, 0666);
  if(fd1 == -1){
    perror("Source file opening failed");
    return -1;
  }
  struct stat attr;
  int status = stat(argv[1], &attr);
  if(status == -1){
    perror("Stat failed");
    return -1;
  }
  off_t length = attr.st_size;
  char *ptr = (char *)mmap(NULL, length, PROT_WRITE|PROT_READ, MAP_PRIVATE, fd1, 0);
  if(ptr == MAP_FAILED){
    perror("Mmap failed");
    return -1;
  }
  close(fd1);
  int fd2 = open(argv[2], O_CREAT|O_TRUNC|O_WRONLY, 0666);
  if(fd1 == -1){
    perror("Destination file opening failed");
    return -1;
  }
  write(fd2, ptr, length);
  close(fd2);
  munmap(ptr, length);
  return 0;
}
