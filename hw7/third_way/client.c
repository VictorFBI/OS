#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>
#include <error.h>
#include <sys/mman.h>

void sys_err(const char *message) {
  perror(message);
  exit(1);
}

const char *SHM_NAME = "myshm";

int main() {
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  char input[256];
  int bytes_read = 0;
  fd_set input_set;
  struct timeval timeout;

  mknod("fifo", S_IFIFO | 0666, 0);
  int f_write = open("fifo", O_WRONLY);
  srand(time(NULL));

  int shm_id;
  int *addr;
  int num;

  if ((shm_id = shm_open(SHM_NAME, O_RDWR, 0666)) == -1) {
    perror("shmget()");
    exit(1);
  }

  addr = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

  if (addr == (int *) -1) {
    printf("Error getting pointer to addrd memory\n");
    return 1;
  }

  printf("addr = %p\n", addr);

  while (1) {
    FD_ZERO(&input_set);
    FD_SET(STDIN_FILENO, &input_set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    if (select(STDIN_FILENO + 1, &input_set, NULL, NULL, &timeout) > 0) {
      bytes_read = read(STDIN_FILENO, input, sizeof(input) - 1);
      if (bytes_read > 0) {
        input[bytes_read] = '\0';
        printf("Was read: %s\n", input);
      }
    }

    num = random() % 1000; // generate
    *addr = num;
    if (bytes_read > 0) {
      if (write(f_write, "stop", 4) < 0) {
        sys_err("client: write error");
      }
      break;
    }
    if (write(f_write, "continue", 8) < 0) {
      sys_err("client: write error");
    }
    printf("write a random number %d\n", num);
    sleep(1);
  }

  close(f_write);
  puts("The client was completed his work\n");
  shmdt(addr);
  return 0;
}
