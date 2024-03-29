#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>
#include<stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include<error.h>
#include "message.h"

void sys_err(const char *message) {
  perror(message);
  exit(1);
}

const char* SHM_NAME = "myshm";

int main() {
  int shm_id;
  message_t *addr;

  if ((shm_id = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) == -1) {
    perror("shmget()");
    exit(1);
  }

  // Задание размера объекта памяти
  if (ftruncate(shm_id, sizeof(message_t)) == -1) {
    perror("ftruncate");
    return 1;
  } else {
    printf("Memory size set and = %lu\n", sizeof(message_t));
  }

  addr = (message_t *) mmap(NULL, sizeof(message_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  addr->status = CONTINUE;

  while (1) {
    sleep(1);
    if (addr->status == STOP) {
      printf("%s\n", "The server has completed his work");
      break;
    }
    printf("%d\n", addr->num);
  }

  if (shm_unlink(SHM_NAME) == -1) {
    printf("addrd memory is absent\n");
    perror("shm_unlink");
    return 1;
  }

  return 0;
}
