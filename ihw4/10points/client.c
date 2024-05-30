#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
  int wins;
  int losses;
  int draws;
} Score;

int main() {
  int sockfd;
  struct sockaddr_in serv_addr;
  socklen_t addrlen = sizeof(struct sockaddr_in);
  char buffer[BUFFER_SIZE];

  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  // Notify the server of the connection
  sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, addrlen);

  Score score;
  recvfrom(sockfd, &score, sizeof(score), 0, (struct sockaddr *)&serv_addr, &addrlen);
  printf("Tournament results:\n");
  printf("Wins: %d\n", score.wins);
  printf("Losses: %d\n", score.losses);
  printf("Draws: %d\n", score.draws);

  close(sockfd);
  return 0;
}
