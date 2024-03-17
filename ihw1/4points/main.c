#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>

bool checkPalindrome(char* str) {
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

  int fd12[2];
  int fd23[2];

  if (pipe(fd12) < 0 || pipe(fd23) < 0) {
    perror("Error with creating pipes");
    exit(EXIT_FAILURE);
  }

  pid_t pidRead = fork();

  if (!pidRead) { // if child
    int fRead = open(argv[1], O_RDONLY);
    char buffer[5000];
    int readBytes = read(fRead, buffer, 5000);
    printf("'%s' was read from the file\n", buffer);
    write(fd12[1], buffer, 5000);
    close(fd12[0]);
    close(fd12[1]);
    close(fd23[0]);
    close(fd23[1]);
    exit(EXIT_SUCCESS);
  }

  pid_t pidSolve = fork();

  if (!pidSolve) { // if child
    char buffer[5000];
    int readBytes = read(fd12[0], buffer, 5000);
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
    if (strlen(ansBuffer) > 0) ansBuffer[strlen(ansBuffer) - 1] = '\0';
    else ansBuffer[0] = ' ';
    write(fd23[1], ansBuffer, strlen(ansBuffer));

    close(fd12[0]);
    close(fd12[1]);
    close(fd23[0]);
    close(fd23[1]);
    exit(EXIT_SUCCESS);
  }

  pid_t pidWrite = fork();

  if (!pidWrite) { // if child
    char buffer[5000];
    int readBytes = read(fd23[0], buffer, 5000);
    if (readBytes < 0) {
      printf("Error with reading from pipe\n");
      exit(EXIT_FAILURE);
    }
    int fWrite = open(argv[2], O_WRONLY | O_CREAT, 0666);
    write(fWrite, buffer, readBytes);
    close(fWrite);
    close(fd12[0]);
    close(fd12[1]);
    close(fd23[0]);
    close(fd23[1]);
    exit(EXIT_SUCCESS);
  }

  for (int i = 0; i < 3; ++i) {
    wait(NULL); // wait until all child processes will finish
  }

  return 0;
}