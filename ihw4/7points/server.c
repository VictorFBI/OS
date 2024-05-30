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
  Score score;
} StudentResult;

int determine_winner(int choice1, int choice2) {
  if (choice1 == choice2) return 0; // Draw
  if ((choice1 == 0 && choice2 == 2) || (choice1 == 1 && choice2 == 0) || (choice1 == 2 && choice2 == 1)) return 1; // Player 1 wins
  return -1; // Player 2 wins
}

void handle_clients(struct sockaddr_in client_addresses[], int total_students, struct sockaddr_in monitor_address, int server_fd) {
  StudentResult results[total_students];
  Score scores[total_students];

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
    results[i].student_id = i;
    results[i].score = scores[i];
    sendto(server_fd, &scores[i], sizeof(scores[i]), 0, (struct sockaddr*)&client_addresses[i], sizeof(client_addresses[i]));
  }

  sendto(server_fd, results, sizeof(results), 0, (struct sockaddr*)&monitor_address, sizeof(monitor_address));
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <number_of_students>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int total_students = atoi(argv[1]);
  if (total_students <= 0 || total_students > MAX_CLIENTS) {
    fprintf(stderr, "Invalid number of students. Must be between 1 and %d.\n", MAX_CLIENTS);
    exit(EXIT_FAILURE);
  }

  int server_fd;
  struct sockaddr_in server_address, client_addresses[MAX_CLIENTS], monitor_address;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  char buffer[BUFFER_SIZE];

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Set up the server address
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);

  // Bind the socket to the port
  if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  printf("Waiting for %d students and one monitor to connect...\n", total_students); // +1 for the monitoring client

  // Accept connections from all students
  for (int i = 0; i < total_students; i++) {
    recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addresses[i], &addr_len);
    printf("Student %d joined the game\n", i + 1);
  }

  // Accept connection from the monitoring client
  printf("Waiting for the monitor to connect...\n");
  recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&monitor_address, &addr_len);
  printf("Monitoring client connected\n");

  printf("All clients connected. Starting the tournament...\n");

  srand(time(NULL));
  handle_clients(client_addresses, total_students, monitor_address, server_fd);

  printf("Tournament finished\n");
  close(server_fd);
  return 0;
}
