#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct {
  int wins;
  int losses;
  int draws;
} Score;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <ip_address> <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const char *ip_address = argv[1];
  int port = atoi(argv[2]);

  int sock = 0;
  struct sockaddr_in serv_addr;
  socklen_t addr_len = sizeof(serv_addr);
  char message[] = "Student connected";

  // Create socket
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  // Convert IPv4 address from text to binary form
  if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  // Send connection message to server
  sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  Score score;
  recvfrom(sock, &score, sizeof(score), 0, (struct sockaddr*)&serv_addr, &addr_len);

  printf("Tournament results:\n");
  printf("Wins: %d\n", score.wins);
  printf("Losses: %d\n", score.losses);
  printf("Draws: %d\n", score.draws);

  close(sock);
  return 0;
}
