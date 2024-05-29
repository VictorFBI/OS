// udp_multicast_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MULTICAST_PORT 8888
#define MULTICAST_IP "239.255.255.250"
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in multicast_addr;
    struct ip_mreq multicast_request;
    char buffer[BUFFER_SIZE];
    int ret;
    socklen_t addr_len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(MULTICAST_PORT);
    multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(sockfd, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
    if (ret < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);
    ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_request, sizeof(multicast_request));
    if (ret < 0) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        addr_len = sizeof(multicast_addr);
        ret = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&multicast_addr, &addr_len);
        if (ret < 0) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        buffer[ret] = '\0';
        printf("Received multicast message: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
