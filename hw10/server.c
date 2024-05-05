#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024
#define PORT 12345

// Функция для получения IP-адреса сервера
void get_server_ip() {
  char host[256];
  struct hostent *host_entry;

  // Получение имени хоста
  if (gethostname(host, sizeof(host)) == -1) {
    perror("Ошибка при получении имени хоста");
    exit(EXIT_FAILURE);
  }

  // Получение информации о хосте
  if ((host_entry = gethostbyname(host)) == NULL) {
    perror("Ошибка при получении информации о хосте");
    exit(EXIT_FAILURE);
  }

  // Вывод IP-адреса сервера
  printf("IP адрес сервера: %s\n", inet_ntoa(*(struct in_addr *)host_entry->h_addr_list[0]));
}

int main() {
  int server_fd, client1_fd, client2_fd;
  struct sockaddr_in server_addr, client1_addr, client2_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  char buffer[BUFFER_SIZE];

  // Создание сокета
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Ошибка при создании сокета");
    exit(EXIT_FAILURE);
  }

  // Заполнение нулями структуры server_addr
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  // Привязка сокета к адресу и порту
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("Ошибка при привязке сокета");
    exit(EXIT_FAILURE);
  }

  // Прослушивание порта
  if (listen(server_fd, 2) == -1) {
    perror("Ошибка при прослушивании порта");
    exit(EXIT_FAILURE);
  }

  // Получение IP-адреса сервера
  get_server_ip();

  printf("Сервер запущен и ожидает подключений...\n");

  // Принятие подключения от клиента №1
  if ((client1_fd = accept(server_fd, (struct sockaddr *)&client1_addr, &addr_len)) == -1) {
    perror("Ошибка при принятии подключения от клиента №1");
    exit(EXIT_FAILURE);
  }

  printf("Клиент №1 подключен.\n");

  // Принятие подключения от клиента №2
  if ((client2_fd = accept(server_fd, (struct sockaddr *)&client2_addr, &addr_len)) == -1) {
    perror("Ошибка при принятии подключения от клиента №2");
    exit(EXIT_FAILURE);
  }

  printf("Клиент №2 подключен.\n");

  // Пересылка сообщений от клиента №1 клиенту №2
  while (1) {
    int bytes_received = recv(client1_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
      perror("Ошибка при чтении данных от клиента №1");
      exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
      printf("Клиент №1 отключился.\n");
      break;
    }

    buffer[bytes_received] = '\0';
    printf("Получено от клиента №1: %s\n", buffer);

    // Пересылка сообщения клиенту №2
    if (send(client2_fd, buffer, strlen(buffer), 0) == -1) {
      perror("Ошибка при отправке данных клиенту №2");
      exit(EXIT_FAILURE);
    }

    // Проверка на завершение работы
    if (strcmp(buffer, "The End") == 0) {
      printf("Работа завершена.\n");
      break;
    }
  }

  // Закрытие сокетов
  close(client1_fd);
  close(client2_fd);
  close(server_fd);

  return 0;
}
