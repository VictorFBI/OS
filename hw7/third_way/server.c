#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <error.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/mman.h>

void sys_err(const char *message) {
  perror(message);
  exit(1);
}

const char *SHM_NAME = "myshm";

int main() {
  char input[256];
  mknod("fifo", S_IFIFO | 0666, 0);
  int f_read = open("fifo", O_RDONLY);

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
    printf("%d\n", *addr);
  }

  close(f_read);
  unlink("fifo");
  puts("EXIT");
  if (shm_unlink(SHM_NAME) == -1) {
    printf("addrd memory is absent\n");
    perror("shm_unlink");
    return 1;
  }

  return 0;
}
