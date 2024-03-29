#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <error.h>
#include <stdbool.h>

void sys_err(const char *message) {
  perror(message);
  exit(1);
}

bool continue_reading = true;
const char *SHM_NAME = "myshm";

void signal_handler(int signum) {
  puts("Signal was received");
  continue_reading = false;
}

int main() {
  printf("My pid is %d\n", getpid());
  signal(SIGTERM, signal_handler);

  int shm_id;
  int *addr;

  if ((shm_id = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) == -1) {
    perror("shmget()");
    exit(1);
  }

  // Задание размера объекта памяти
  if (ftruncate(shm_id, sizeof(int)) == -1) {
    perror("ftruncate");
    return 1;
  } else {
    printf("Memory size set and = %lu\n", sizeof(int));
  }

  addr = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

  while (continue_reading) {
    sleep(1);
    if (continue_reading) printf("%d\n", *addr);
  }

  puts("EXIT");
  if (shm_unlink(SHM_NAME) == -1) {
    printf("addrd memory is absent\n");
    perror("shm_unlink");
    return 1;
  }

  return 0;
}
