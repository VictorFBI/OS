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
  mknod("fifo1.fifo", S_IFIFO | 0666, 0);
  mknod("fifo2.fifo", S_IFIFO | 0666, 0);

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


  return 0;
}
