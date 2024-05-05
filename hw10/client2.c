#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Использование: %s <IP-адрес сервера> <порт сервера>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *server_ip = argv[1];
  int server_port = atoi(argv[2]);

  int client_fd;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];

  // Создание сокета
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Ошибка при создании сокета");
    exit(EXIT_FAILURE);
  }

  // Заполнение структуры server_addr
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(server_ip);
  server_addr.sin_port = htons(server_port);

  // Подключение к серверу
  if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("Ошибка при подключении к серверу");
    exit(EXIT_FAILURE);
  }

  printf("Подключение к серверу выполнено.\n");

  // Прием и вывод сообщений от сервера
  while (1) {
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
      perror("Ошибка при чтении данных от сервера");
      exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
      printf("Сервер отключился.\n");
      break;
    }

    buffer[bytes_received] = '\0';
    printf("Получено от сервера: %s\n", buffer);

    // Проверка на завершение работы
    if (strcmp(buffer, "The End") == 0) {
      printf("Работа завершена.\n");
      break;
    }
  }

  // Закрытие сокета
  close(client_fd);

  return 0;
}
