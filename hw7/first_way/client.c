#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <time.h>
#include <error.h>
#include "message.h"

const char* SHM_NAME = "myshm";

int main() {
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  char input[MAX_MESSAGE_SIZE];
  int bytes_read = 0;
  fd_set input_set;
  struct timeval timeout;


  int shm_id;
  message_t *addr;
  int num;

  if ((shm_id = shm_open(SHM_NAME, O_RDWR, 0666)) == -1) {
    perror("shmget()");
    exit(1);
  }

  addr = (message_t *) mmap(NULL, sizeof(message_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

  if (addr == (message_t *)-1 ) {
    printf("Error getting pointer to addrd memory\n");
    return 1;
  }


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
    addr->num = num;
    if (bytes_read > 0) {
      addr->status = STOP;
      break;
    }
    printf("write a random number %d\n", addr->num);
    sleep(1);
  }

  puts("The client was completed his work\n");
  return 0;
}
