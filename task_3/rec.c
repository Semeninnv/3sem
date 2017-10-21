#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
int main(){
  key_t key = ftok("./sin.c", 0);
  struct sembuf sbuf;
  sbuf.sem_flg = 0;
  sbuf.sem_num = 0;
  sbuf.sem_op = -1;
  int semid = semget(key, 1, IPC_CREAT | 0666);
  int shmid = shmget(key, sizeof(double), 0666 | IPC_CREAT);
  double *shm = (double *)shmat(shmid, NULL, 0);
  write(1, "Waiting for the calculator...", 30);
  semop(semid, &sbuf, 1);
  printf("done.\n%lf\n", (*shm));
  shmdt((char *)shm);
  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 0, IPC_RMID, 0);
  return 0;
}
