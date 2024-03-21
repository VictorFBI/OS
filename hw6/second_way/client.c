// shmem-gen.c
// write a random number between 0 and 999 to the shm every 1 second
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>
#include <time.h>
#include <error.h>

int main() {
  int shm_id;
  int *share;

  pid_t server_pid;
  printf("Enter the server pid: ");
  scanf("%d", &server_pid);

  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  char input[256];
  int bytes_read = 0;
  fd_set input_set;
  struct timeval timeout;

  srand(time(NULL));
  shm_id = shmget(0x2FF, getpagesize(), 0666 | IPC_CREAT);
  printf("shm_id = %d\n", shm_id);
  if (shm_id < 0) {
    perror("shmget()");
    exit(1);
  }

  /* подключение сегмента к адресному пространству процесса */
  share = (int *) shmat(shm_id, 0, 0);
  if (share == NULL) {
    perror("shmat()");
    exit(2);
  }

  printf("share = %p\n", share);
  int num;
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
    if (bytes_read > 0) {
      kill(server_pid, SIGTERM);
      puts("Signal was sent\n");
      break;
    }
    *share = num;
    printf("write a random number %d\n", num);
    sleep(1);
  }

  puts("The client was completed his work\n");
  shmdt(share);
  return 0;
}
