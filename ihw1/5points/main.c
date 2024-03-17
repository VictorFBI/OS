#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>

bool checkPalindrome(char *str) {
  int right = strlen(str) - 1;
  int left = 0;
  while (left < right) {
    if (str[left] != str[right]) return false;
    ++left;
    --right;
  }
  return true;
}

int main(int argc, char *argv[]) {
  if (access(argv[1], F_OK)) {
    printf("No such source file exists\n");
    exit(EXIT_FAILURE);
  }
  char fifo12[] = "fifo12.fifo";
  char fifo23[] = "fifo23.fifo";

  if (mknod(fifo12, S_IFIFO | 0666, 0) < 0) {
    printf("Can\'t create FIFO\n");
    exit(-1);
  }

  if (mknod(fifo23, S_IFIFO | 0666, 0) < 0) {
    printf("Can\'t create FIFO\n");
    exit(-1);
  }

  pid_t pidRead = fork();

  if (!pidRead) { // if child
    int fRead = open(argv[1], O_RDONLY);
    char buffer[5000];
    int readBytes = read(fRead, buffer, 5000);
    printf("'%s' was read from the file\n", buffer);
    close(fRead);
    fRead = open(fifo12, O_WRONLY);
    write(fRead, buffer, readBytes);
    close(fRead);

    exit(EXIT_SUCCESS);
  }

  pid_t pidSolve = fork();

  if (!pidSolve) { // if child
    char buffer[5000];
    int fRead = open(fifo12, O_RDONLY);
    int readBytes = read(fRead, buffer, 5000);
    close(fRead);
    printf("'%s' was read from the pipe\n", buffer);
    char ansBuffer[5000];
    char tmpBuffer[5000];
    int ind = 0;
    int size = strlen(buffer);
    for (int i = 0; i < size; ++i) {
      while ((65 <= buffer[i] && buffer[i] <= 90) || (97 <= buffer[i] && buffer[i] <= 122)) {
        tmpBuffer[ind++] = buffer[i++];
      }
      if (strlen(tmpBuffer) == 0) continue;
      if (checkPalindrome(tmpBuffer)) {
        strcat(ansBuffer, tmpBuffer);
        strcat(ansBuffer, " ");
      }
      ind = 0;
      memset(tmpBuffer, 0, 5000);
    }
    ansBuffer[strlen(ansBuffer) - 1] = '\0';
    int fWrite = open(fifo23, O_WRONLY);
    write(fWrite, ansBuffer, strlen(ansBuffer));
    close(fWrite);
    exit(EXIT_SUCCESS);
  }

  pid_t pidWrite = fork();

  if (!pidWrite) { // if child
    char buffer[5000];
    int fRead = open(fifo23, O_RDONLY);
    int readBytes = read(fRead, buffer, 5000);
    int fWrite = open(argv[2], O_WRONLY | O_CREAT, 0666);
    write(fWrite, buffer, readBytes);
    close(fRead);
    close(fWrite);

    exit(EXIT_SUCCESS);
  }

  for (int i = 0; i < 3; ++i) {
    wait(NULL); // wait until all child processes will finish
  }

  unlink(fifo12);
  unlink(fifo23);

  return 0;
}