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
  int fdWrite = open("fifo2.fifo", O_WRONLY);
  char buffer[3];
  int readBytes;
  while(true) {
    readBytes = read(fdRead, buffer, 3);
    if (readBytes == 0) break;
    printf("'%.*s' was read from the pipe\n", readBytes, buffer);

    char ansBuffer[5000]; // this buffer is not related for reading or writing to the file, so its size can be 5000
    char tmpBuffer[5000]; // this buffer is not related for reading or writing to the file, so its size can be 5000
    int ind = 0;
    int size = strlen(buffer);
    for (int i = 0; i < readBytes; ++i) {
      while (i < readBytes && ((65 <= buffer[i] && buffer[i] <= 90) || (97 <= buffer[i] && buffer[i] <= 122))) {
        tmpBuffer[ind++] = buffer[i++];
      }
      if (strlen(tmpBuffer) == 0) continue;
      --i;
      if (checkPalindrome(tmpBuffer)) {
        strcat(ansBuffer, tmpBuffer);
        strcat(ansBuffer, " ");
      }
      ind = 0;
      memset(tmpBuffer, 0, 5000);
    }
    ansBuffer[strlen(ansBuffer) - 1] = '&';
    ansBuffer[strlen(ansBuffer)] = '\0';

    write(fdWrite, ansBuffer, strlen(ansBuffer));

    printf("'%s' was sent to the pipe\n", ansBuffer);
    memset(ansBuffer, 0, 5000);
  }

  close(fdRead);
  close(fdWrite);

  return 0;
}
