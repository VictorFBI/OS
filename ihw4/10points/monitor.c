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
  int wins;
  int losses;
  int draws;
} StudentResult;

int main() {
  int sockfd;
  struct sockaddr_in serv_addr;
  socklen_t addrlen = sizeof(struct sockaddr_in);
  char buffer[BUFFER_SIZE];
  StudentResult results[MAX_CLIENTS];
  int total_students;

  printf("Enter the number of students: ");
  scanf("%d", &total_students);

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

  recvfrom(sockfd, results, sizeof(results), 0, (struct sockaddr *)&serv_addr, &addrlen);

  printf("Overall tournament results:\n");
  for (int i = 0; i < total_students; i++) {
    printf("Student %d - Wins: %d, Losses: %d, Draws: %d\n", results[i].student_id, results[i].wins, results[i].losses, results[i].draws);
  }

  close(sockfd);
  return 0;
}
