// shmem-gen.c
// write a random number between 0 and 999 to the shm every 1 second
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>
#include <error.h>
#include "message.h"


int main() {
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  char input[MAX_MESSAGE_SIZE];
  int bytes_read = 0;
  fd_set input_set;
  struct timeval timeout;


  int shm_id;
  message_t *share;
  int num;

  srand(time(NULL));
  shm_id = shmget(0x2FF, getpagesize(), 0666 | IPC_CREAT);
  printf("shm_id = %d\n", shm_id);
  if (shm_id < 0) {
    perror("shmget()");
    exit(1);
  }

  /* подключение сегмента к адресному пространству процесса */
  share = (message_t *) shmat(shm_id, 0, 0);
  if (share == NULL) {
    perror("shmat()");
    exit(2);
  }
  printf("share = %p\n", share);
  share->status = CONTINUE;
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
    share->num = num;
    if (bytes_read > 0) {
      share->status = STOP;
      break;
    }
    printf("write a random number %d\n", share->num);
    sleep(1);
  }

  puts("The client was completed his work\n");
  shmdt(share);
  return 0;
}
