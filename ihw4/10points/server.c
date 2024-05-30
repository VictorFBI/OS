#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

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

int determine_winner(int choice1, int choice2) {
  if (choice1 == choice2) return 0; // Draw
  if ((choice1 == 0 && choice2 == 2) || (choice1 == 1 && choice2 == 0) || (choice1 == 2 && choice2 == 1)) return 1; // Player 1 wins
  return -1; // Player 2 wins
}

void handle_clients(struct sockaddr_in client_addresses[], int total_students, struct sockaddr_in monitor_addresses[], int total_monitors, int sockfd) {
  StudentResult results[total_students];
  Score scores[total_students];
  socklen_t addrlen = sizeof(struct sockaddr_in);
  char buffer[BUFFER_SIZE];

  for (int i = 0; i < total_students; i++) {
    scores[i].wins = 0;
    scores[i].losses = 0;
    scores[i].draws = 0;
  }

  for (int i = 0; i < total_students; i++) {
    for (int j = i + 1; j < total_students; j++) {
      int choice1 = rand() % 3;
      int choice2 = rand() % 3;
      int result = determine_winner(choice1, choice2);

      if (result == 1) {
        scores[i].wins++;
        scores[j].losses++;
      } else if (result == -1) {
        scores[i].losses++;
        scores[j].wins++;
      } else {
        scores[i].draws++;
        scores[j].draws++;
      }
    }
  }

  for (int i = 0; i < total_students; i++) {
    results[i].student_id = i + 1;
    results[i].wins = scores[i].wins;
    results[i].losses = scores[i].losses;
    results[i].draws = scores[i].draws;
    sendto(sockfd, &scores[i], sizeof(scores[i]), 0, (struct sockaddr *)&client_addresses[i], addrlen);
  }

  for (int i = 0; i < total_monitors; i++) {
    sendto(sockfd, results, sizeof(results), 0, (struct sockaddr *)&monitor_addresses[i], addrlen);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <number_of_students> <number_of_monitors>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int total_students = atoi(argv[1]);
  int total_monitors = atoi(argv[2]);
  if (total_students <= 0 || total_students > MAX_CLIENTS) {
    fprintf(stderr, "Invalid number of students. Must be between 1 and %d.\n", MAX_CLIENTS);
    exit(EXIT_FAILURE);
  }

  int sockfd;
  struct sockaddr_in server_addr, client_addresses[MAX_CLIENTS], monitor_addresses[MAX_CLIENTS];
  socklen_t addrlen = sizeof(struct sockaddr_in);
  char buffer[BUFFER_SIZE];

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind the socket to the port
  if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Waiting for %d students to connect...\n", total_students);
  for (int i = 0; i < total_students; i++) {
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addresses[i], &addrlen);
    printf("Student %d connected.\n", i + 1);
  }

  printf("Waiting for %d monitors to connect...\n", total_monitors);
  for (int i = 0; i < total_monitors; i++) {
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&monitor_addresses[i], &addrlen);
    printf("Monitor %d connected.\n", i + 1);
  }

  printf("All clients connected. Starting the tournament...\n");

  srand(time(NULL));
  handle_clients(client_addresses, total_students, monitor_addresses, total_monitors, sockfd);

  printf("Tournament finished.\n");
  close(sockfd);
  return 0;
}
