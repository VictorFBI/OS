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

  mknod("fifo1.fifo", S_IFIFO | 0666, 0);
  mknod("fifo2.fifo", S_IFIFO | 0666, 0);

  pid_t pidReadWrite = fork();

  if (pidReadWrite < 0) {
    perror("Error with creating fork");
    exit(EXIT_FAILURE);
  }

  if (pidReadWrite) { // if parent (read-write)
    int fRead = open(argv[1], O_RDONLY);
    char buffer[5000];
    int readBytes = read(fRead, buffer, 5000);
    if (readBytes == 0) {
      buffer[0] = ' ';
      ++readBytes;
    }
    printf("'%s' was read from the file\n", buffer);

    int fdWrite = open("fifo1.fifo", O_WRONLY);
    write(fdWrite, buffer, readBytes);
    close(fdWrite);
    close(fRead);

    int fdRead = open("fifo2.fifo", O_RDONLY);
    char tmpBuffer[5000];
    readBytes = read(fdRead, tmpBuffer, 5000);
    int fWrite = open(argv[2], O_WRONLY | O_CREAT, 0666);
    write(fWrite, tmpBuffer, readBytes);
    close(fWrite);
    close(fdRead);
    printf("'%s' was written to the file\n", tmpBuffer);

    exit(EXIT_SUCCESS);
  } else { // if child (task solving)
    int fdRead = open("fifo1.fifo", O_RDONLY);
    char buffer[5000];
    int readBytes = read(fdRead, buffer, 5000);
    close(fdRead);
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

    int fdWrite = open("fifo2.fifo", O_WRONLY);
    write(fdWrite, ansBuffer, strlen(ansBuffer));
    close(fdWrite);
    printf("'%s' was sent to the pipe\n", ansBuffer);

    exit(EXIT_SUCCESS);
  }

  return 0;
}
