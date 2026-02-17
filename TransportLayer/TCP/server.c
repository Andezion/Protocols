#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tcp.h"

int main(void) {
    uint16_t port = 12345; // тут просто выбрали порт, можно любой выбрать от 1024 до 65535, но он должен быть свободным
    int s = udp_socket_bind(port); // тут его бронируем

    if (s < 0) { // если порт занят или где-то другая ошибка 
        perror("bind");
        return 1;
    }

    printf("Server listening on UDP port %u (simulated TCP)\n", port);

    struct sockaddr_storage client;

    socklen_t clientlen = sizeof(client);
    uint32_t client_isn, server_isn;

    if (tcp_accept(s, (struct sockaddr*)&client, &clientlen, &client_isn, &server_isn) == 0) {
        char host[INET6_ADDRSTRLEN];

        if (client.ss_family == AF_INET) {
            struct sockaddr_in *a = (struct sockaddr_in *) &client;

            inet_ntop(AF_INET, &a->sin_addr, host, sizeof(host));
            printf("Accepted connection from %s: %u\n", host, ntohs(a->sin_port));
        }

        printf("Client ISN = %u, Server ISN = %u\n", client_isn, server_isn);
        printf("Handshake complete - connection established.\n");
    } else {
        printf("Handshake failed or timed out.\n");
    }

    close(s);
    return 0;
}