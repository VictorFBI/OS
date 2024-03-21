// shmem-out.c
// read from the shm every 1 second
#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdlib.h>
#include<error.h>

void sys_err(const char *message) {
  perror(message);
  exit(1);
}

int main() {
  int shm_id;
  int *share;

  char input[256];
  mknod("fifo", S_IFIFO | 0666, 0);
  int f_read = open("fifo", O_RDONLY);

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
  int read_bytes;

  while (1) {
    sleep(1);
    if ((read_bytes = read(f_read, input, 256)) < 0) {
      sys_err("server: read error");
    }
    if (read_bytes == 4) {
      printf("%s\n", "The server has completed his work");
      break;
    }
    printf("%d\n", *share);
  }

  close(f_read);
  unlink("fifo");
  shmdt(share);
  if (shmctl(shm_id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
    sys_err("server: shared memory remove error");
  }

  return 0;
}
