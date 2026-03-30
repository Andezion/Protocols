#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "udp.h"

int main() {
    const char *server_ip = "127.0.0.1";
    uint16_t port = 12345;

    // Создаем UDP-сокет и привязываем его к порту
    int sockfd = udp_socket_bind(port);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create and bind UDP socket.\n");
        return EXIT_FAILURE;
    }

    printf("UDP client is ready to send messages to %s:%u\n", server_ip, port);
    
    // Структура для хранения адреса сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // Сообщение для отправки
    const char *message = "Hello, UDP server!";
    ssize_t sent_bytes = udp_sendto(sockfd, message, strlen(message), (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent_bytes < 0) {
        fprintf(stderr, "Failed to send message to UDP server.\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

        printf("Sent message to UDP server: %s\n", message);
    // Буфер для получения ответа от сервера
    char buffer[UDP_MAX_PAYLOAD];
    struct sockaddr_in src_addr;
    socklen_t src_addr_len = sizeof(src_addr);
    ssize_t recv_bytes = udp_recvfrom(sockfd, buffer, sizeof(buffer) - 1, (struct sockaddr *)&src_addr, &src_addr_len);
    if (recv_bytes < 0) {
        fprintf(stderr, "Failed to receive message from UDP server.\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    buffer[recv_bytes] = '\0'; // Добавляем нулевой терминатор к полученному сообщению
    printf("Received message from UDP server: %s\n", buffer);
    // Закрываем сокет
    close(sockfd);


    return 0;
}