// udp_broadcast_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in broadcast_addr;
    char buffer[BUFFER_SIZE];
    int ret;
    socklen_t addr_len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(BROADCAST_PORT);
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(sockfd, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
    if (ret < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        addr_len = sizeof(broadcast_addr);
        ret = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&broadcast_addr, &addr_len);
        if (ret < 0) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        buffer[ret] = '\0';
        printf("Received broadcast message: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
