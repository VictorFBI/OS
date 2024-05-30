#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

typedef struct {
  int wins;
  int losses;
  int draws;
} Score;

int determine_winner(int choice1, int choice2) {
  if (choice1 == choice2) return 0; // Draw
  if ((choice1 == 0 && choice2 == 2) || (choice1 == 1 && choice2 == 0) || (choice1 == 2 && choice2 == 1)) return 1; // Player 1 wins
  return -1; // Player 2 wins
}

void handle_clients(struct sockaddr_in client_addresses[], int client_count, int server_fd) {
  Score scores[MAX_CLIENTS];
  for (int i = 0; i < client_count; i++) {
    scores[i].wins = 0;
    scores[i].losses = 0;
    scores[i].draws = 0;
  }

  for (int i = 0; i < client_count; i++) {
    for (int j = i + 1; j < client_count; j++) {
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

  for (int i = 0; i < client_count; i++) {
    sendto(server_fd, &scores[i], sizeof(scores[i]), 0, (struct sockaddr*)&client_addresses[i], sizeof(client_addresses[i]));
  }
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <ip_address> <port> <number_of_students>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const char* ip_address = argv[1];
  int port = atoi(argv[2]);
  int total_students = atoi(argv[3]);

  if (total_students <= 0 || total_students > MAX_CLIENTS) {
    fprintf(stderr, "Invalid number of students. Must be between 1 and %d.\n", MAX_CLIENTS);
    exit(EXIT_FAILURE);
  }

  int server_fd;
  struct sockaddr_in server_address, client_addresses[MAX_CLIENTS];
  char buffer[BUFFER_SIZE];
  socklen_t client_address_len = sizeof(struct sockaddr_in);

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Set up the server address
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(ip_address);;
  server_address.sin_port = htons(port);

  // Bind the socket to the port
  if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  printf("Waiting for %d students to connect...\n", total_students);

  // Accept connections from all students
  for (int i = 0; i < total_students; i++) {
    recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addresses[i], &client_address_len);
    printf("Student %d joined the game\n", i + 1);
  }

  printf("All students connected. Starting the tournament...\n");

  srand(time(NULL));
  handle_clients(client_addresses, total_students, server_fd);

  printf("Tournament finished\n");
  close(server_fd);
  return 0;
}
