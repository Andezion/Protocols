#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    uint16_t port = 12345;

    // Создаем UDP-сокет и привязываем его к порту
    int sockfd = udp_socket_bind(port);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create and bind UDP socket.\n");
        return EXIT_FAILURE;
    }

    printf("UDP server is listening on port %u\n", port);
    char buffer[UDP_MAX_PAYLOAD];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Получаем данные от клиента
    ssize_t recv_bytes = udp_recvfrom(sockfd, buffer, sizeof(buffer) - 1, (struct sockaddr *)&client_addr, &client_addr_len);
    if (recv_bytes < 0) {
        fprintf(stderr, "Failed to receive message from client.\n");
    }

    buffer[recv_bytes] = '\0'; // Добавляем нулевой терминатор к полученному сообщению
    printf("Received message from client: %s\n", buffer);

    // Отправляем ответ клиенту
    const char *reply = "Hello from UDP server!";
    ssize_t sent_bytes = udp_sendto(sockfd, reply, strlen(reply), (struct sockaddr *)&client_addr, client_addr_len);
    if (sent_bytes < 0) {
        fprintf(stderr, "Failed to send message to client.\n");
    }

// Закрываем сокет (хотя в этом примере мы никогда не достигнем этой строки)
    close(sockfd);
    return EXIT_SUCCESS;
}