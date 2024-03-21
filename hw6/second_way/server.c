// shmem-out.c
// read from the shm every 1 second
#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<error.h>
#include<signal.h>
#include<stdbool.h>

void sys_err(const char *message) {
  perror(message);
  exit(1);
}

bool continue_reading = true;

void signal_handler(int signum) {
  puts("Signal was received");
  continue_reading = false;
}

int main() {
  int shm_id;
  int *share;
  printf("My pid is %d\n", getpid());
  signal(SIGTERM, signal_handler);

  shm_id = shmget(0x2FF, getpagesize(), 0666 | IPC_CREAT);
  if (shm_id == -1) {
    perror("shmget()");
    exit(1);
  }

  share = (int *) shmat(shm_id, 0, 0);
  if (share == NULL) {
    perror("shmat()");
    exit(2);
  }

  while (continue_reading) {
    sleep(1);
    if (continue_reading) printf("%d\n", *share);
  }

  puts("EXIT");
  shmdt(share);
  if (shmctl(shm_id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
    sys_err("server: shared memory remove error");
  }

  return 0;
}
