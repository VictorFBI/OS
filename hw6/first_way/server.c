// shmem-out.c
// read from the shm every 1 second
#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<error.h>
#include "message.h"

void sys_err(const char *message) {
  perror(message);
  exit(1);
}

int main() {
  int shm_id;
  message_t *share;

  shm_id = shmget(0x2FF, getpagesize(), 0666 | IPC_CREAT);
  if (shm_id == -1) {
    perror("shmget()");
    exit(1);
  }

  share = (message_t *) shmat(shm_id, 0, 0);
  if (share == NULL) {
    perror("shmat()");
    exit(2);
  }
  share->status = CONTINUE;
  while (1) {
    sleep(1);
    if (share->status == STOP) {
      printf("%s\n", "The server has completed his work");
      break;
    }
    printf("%d\n", share->num);
  }

  shmdt(share);
  if (shmctl(shm_id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
    sys_err("server: shared memory remove error");
  }

  return 0;
}
