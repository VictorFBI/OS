// udp_multicast_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_PORT 8888
#define MULTICAST_IP "239.255.255.250"

int main() {
    int sockfd;
    struct sockaddr_in multicast_addr;
    int ret;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(MULTICAST_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);

    while (1) {
        char message[1024];
        fgets(message, sizeof(message), stdin);
        ret = sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
        if (ret < 0) {
            perror("sendto");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        printf("Multicast message sent\n");
        sleep(1);
    }

    close(sockfd);
    return 0;
}
