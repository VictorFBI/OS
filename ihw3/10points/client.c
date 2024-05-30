#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

typedef struct {
  int wins;
  int losses;
  int draws;
} Score;

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;

  // Create socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  // Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }

  Score score;
  read(sock, &score, sizeof(score));
  printf("Tournament results:\n");
  printf("Wins: %d\n", score.wins);
  printf("Losses: %d\n", score.losses);
  printf("Draws: %d\n", score.draws);

  close(sock);
  return 0;
}
