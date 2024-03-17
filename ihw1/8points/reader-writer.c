#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>


int main(int argc, char *argv[]) {
  if (access(argv[1], F_OK)) {
    printf("No such source file exists\n");
    exit(EXIT_FAILURE);
  }

  mknod("fifo1.fifo", S_IFIFO | 0666, 0);
  mknod("fifo2.fifo", S_IFIFO | 0666, 0);

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


  return 0;
}
