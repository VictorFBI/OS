#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

typedef struct {
  int wins;
  int losses;
  int draws;
} Score;

typedef struct {
  int student_id;
  Score score;
} StudentResult;

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  StudentResult results[MAX_CLIENTS];
  int total_students;
  socklen_t addr_len = sizeof(serv_addr);
  char message[] = "Monitor connected";

  printf("Enter the number of students: ");
  scanf("%d", &total_students);

  // Create socket
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 addresses from text to binary form
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  // Send connection message to server
  sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  recvfrom(sock, results, sizeof(results), 0, (struct sockaddr*)&serv_addr, &addr_len);

  printf("Overall tournament results:\n");
  for (int i = 0; i < total_students; i++) {
    printf("Student %d - Wins: %d, Losses: %d, Draws: %d\n", results[i].student_id, results[i].score.wins, results[i].score.losses, results[i].score.draws);
  }

  close(sock);
  return 0;
}
