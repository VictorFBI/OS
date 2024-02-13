#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

const int buffSizeLarge = 4096;
const int buffSizeSmall = 16;

int main(int argc, char* argv[]) {
  if (std::atoi(argv[1]) != 2) {
    printf("Wrong input!\n");
    return 0;
  }

  struct stat fileStat;
  if (stat(argv[2], &fileStat) < 0) { // collecting information about the file in the variable
    printf("An error has occurred while processing the file");
    exit(-1);
  }


  //int size = buffSizeLarge;
  int size = buffSizeSmall;

  int fdRead = open(argv[2], O_RDONLY);
  int fdWrite = open(argv[3], O_WRONLY | O_CREAT, fileStat.st_mode); // creating a new file with access rights of the first file

  char buffer[size];
  ssize_t readBytes;

  if (fdRead < 0) {
    printf("Cannot open the read-file\n");
    exit(-1);
  }

  if (fdWrite < 0) {
    printf("Cannot open the write-file\n");
    exit(-1);
  }

  if (size == buffSizeLarge) {
    readBytes = read(fdRead, buffer, size);
    write(fdWrite, buffer, readBytes);
  } else {
    do {
      readBytes = read(fdRead, buffer, size);
      if (readBytes < 0) {
        printf("Cannot read the file!\n");
        exit(-1);
      }
      write(fdWrite, buffer, readBytes);
    } while (readBytes == size);
  }

  if (close(fdRead) < 0) {
    printf("Cannot close the read-file");
    exit(-1);
  }

  if (close(fdWrite) < 0) {
    printf("Cannot close the write-file");
    exit(-1);
  }

  return 0;
}
