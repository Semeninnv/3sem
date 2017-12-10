#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <poll.h>
void write_all(int fd, char *buf, int length){
  int written;
  int bytes = length;
  char *ptr = buf;
  while(1){
    written = write(fd, ptr, bytes);
    bytes = bytes - written;
    if(bytes == 0)
      break;
    ptr = ptr + written;
  }
}
void event_handler(int fd, char *argv[], int flog){
  char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
  struct inotify_event *event;
  char *ptr;
  char s[100];
  int length;
  time_t t;
  struct tm *tm;
  while(1){
    length = read(fd, buf, sizeof(buf));
    time(&t);
    tm = localtime(&t);
    if (length == 0)
      break;
    strftime(s, 100, "%d.%m.%Y %H:%M:%S", tm);
    for(ptr = buf; ptr < buf + length; ptr += sizeof(struct inotify_event) + event->len){
      event = (struct inotify_event *)ptr;
      write_all(flog, s, strlen(s));
      if(event->mask & IN_ACCESS)
        write_all(flog, " IN_ACCESS: ", strlen(" IN_ACCESS: "));
      if(event->mask & IN_ATTRIB)
        write_all(flog, " IN_ATTRIB: ", strlen(" IN_ATTRIB: "));
      if(event->mask & IN_CLOSE)
        write_all(flog, " IN_CLOSE: ", strlen(" IN_CLOSE: "));
      if(event->mask & IN_CREATE)
        write_all(flog, " IN_CREATE: ", strlen(" IN_CREATE: "));
      if(event->mask & IN_DELETE)
        write_all(flog, " IN_DELETE: ", strlen(" IN_DELETE: "));
      if(event->mask & IN_DELETE_SELF)
        write_all(flog, " IN_DELETE_SELF: ", strlen(" IN_DELETE_SELF: "));
      if(event->mask & IN_MODIFY)
        write_all(flog, " IN_MODIFY: ", strlen(" IN_MODIFY: "));
      if(event->mask & IN_MOVE_SELF)
        write_all(flog, " IN_MOVE_SELF: ", strlen(" IN_MOVE_SELF: "));
      if(event->mask & IN_MOVED_FROM)
        write_all(flog, " IN_MOVED_FROM: ", strlen(" IN_MOVED_FROM: "));
      if(event->mask & IN_MOVED_TO)
        write_all(flog, " IN_MOVED_TO: ", strlen(" IN_MOVED_TO: "));
      if(event->mask & IN_OPEN)
        write_all(flog, " IN_OPEN: ", strlen(" IN_OPEN: "));
      write_all(flog, argv[1], strlen(argv[1]));
      write_all(flog, "/", 1);
      if(event->len){
        write_all(flog, event->name, strlen(event->name));
        write_all(flog, "\n", 1);
      }
      else{
        write_all(flog,"\n", 1);
      }
    }
  }
}
int main(int argc, char *argv[]){
  pid_t pid = fork();
  if(pid > 0){
    exit(0);
  }
  if(pid < 0){
    perror("Fork error");
    exit(-1);
  }
  if(pid == 0){ //the daemon himself
    setsid();
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_SETMASK, &set, NULL);
    struct pollfd pollfd[1];
    int poll_result;
    int flog = open("/tmp/inotify.log", O_CREAT|O_WRONLY|O_APPEND, 0777);
    int fpid = open("/tmp/inotify.pid", O_CREAT|O_TRUNC|O_WRONLY, 0777);
    dprintf(fpid, "%d\n", (int)getpid());
    close(fpid);
    int fd = inotify_init();
    pollfd[0].fd = fd;
    pollfd[0].events = POLLIN;
    int wd = inotify_add_watch(fd, argv[1], IN_ALL_EVENTS);
    while(1){
      poll_result = poll(pollfd, 1, -1);
      if (poll_result > 0)
        event_handler(fd, argv, flog);
    }
  }
  return 0;
}
