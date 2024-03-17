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
  int fdWrite = open("fifo1.fifo", O_WRONLY);
  char buffer[3];
  int readBytes;
  while (true) {
    readBytes = read(fRead, buffer, 3);
    if (readBytes == 0) break;
    printf("'%.*s' was read from the file and sent to the pipe\n", readBytes, buffer);
    write(fdWrite, buffer, readBytes);
  }


  close(fdWrite);
  close(fRead);

  int fdRead = open("fifo2.fifo", O_RDONLY);
  int fWrite = open(argv[2], O_WRONLY | O_CREAT, 0666);
  char tmpBuffer[3];
  while (true) {
    readBytes = read(fdRead, tmpBuffer, 3);
    if (readBytes == 0) break;
    write(fWrite, tmpBuffer, readBytes);
    printf("'%.*s' was written to the file\n", readBytes, tmpBuffer);
    memset(tmpBuffer, 0, 3);
  }

  close(fWrite);
  close(fdRead);

  return 0;
}
