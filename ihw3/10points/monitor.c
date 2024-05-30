#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 100

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
  int sock = 0;
  struct sockaddr_in serv_addr;
  StudentResult results[MAX_CLIENTS];
  int total_students;

  printf("Enter the number of students: ");
  scanf("%d", &total_students);

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

  recv(sock, results, sizeof(results), 0);

  printf("Overall tournament results:\n");
  for (int i = 0; i < total_students; i++) {
    printf("Student %d - Wins: %d, Losses: %d, Draws: %d\n", results[i].student_id, results[i].wins, results[i].losses, results[i].draws);
  }

  close(sock);
  return 0;
}
