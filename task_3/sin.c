#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>
double *shm;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long long int N;
void *carl(void *arg){
  srand(clock());
  double *dest = (double *)arg;
  unsigned long long i, s = 0;
  double x, y;
  for (i = 1; i <= N; i++){
    x = (double)(M_PI * (double)rand()/RAND_MAX);
    y = (double)rand()/RAND_MAX;
    if (y <= sin(x))
      s++;
  }
  pthread_mutex_lock(&mutex);
  (*dest) += M_PI * s/N;
  pthread_mutex_unlock(&mutex);
  pthread_exit((void *)NULL);
}
int main(int argc, char *argv[]){
  struct sembuf sbuf;
  sbuf.sem_flg = 0;
  sbuf.sem_num = 0;
  sbuf.sem_op = 1;
  double s = 0;
  int shmid, j;
  key_t key;
  unsigned int n;
  n = atoi(argv[1]);
  pthread_t tid[n];
  N = atoll(argv[2]) / n;
  key = ftok("./sin.c", 0);
  int semid = semget(key, 1, IPC_CREAT | 0666);
  shmid = shmget(key, sizeof(double), 0666 | IPC_CREAT);
  shm = (double *)shmat(shmid, NULL, 0);
  (*shm) = 0;
  for (j = 0; j < n; j++){
    pthread_create(&tid[j], (pthread_attr_t *)NULL, carl, (void*) shm);
  }
  for (j = 0; j < n; j++){
    pthread_join(tid[j], (void **)NULL);
  }
  (*shm) = (*shm) / n;
  semop(semid, &sbuf, 1);
  shmdt((char *)shm);
  return 0;
}
