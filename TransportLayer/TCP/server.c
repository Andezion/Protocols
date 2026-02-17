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

    struct sockaddr_storage client; // храним адрес клиента, который подключается

    socklen_t clientlen = sizeof(client); // храним длину адреса клиента, она может быть разной для IPv4 и IPv6, поэтому мы используем socklen_t, который достаточно большой для хранения любого адреса
    uint32_t client_isn, server_isn; // храним ISN клиента и сервера, которые мы получаем в процессе рукопожатия

    // ждем входящее соединение, если рукопожатие прошло успешно, то мы заполняем client, clientlen, client_isn и server_isn, иначе мы возвращаем -1
    if (tcp_accept(s, (struct sockaddr*)&client, &clientlen, &client_isn, &server_isn) == 0) {
        char host[INET6_ADDRSTRLEN]; // буфер для хранения строки с IP адресом клиента, мы используем INET6_ADDRSTRLEN, который достаточно большой для хранения любого IPv4 или IPv6 адреса в текстовом виде

        // если клиент использует IPv4, то мы преобразуем его адрес в строку и выводим его на экран, иначе мы просто говорим, что мы приняли соединение от клиента без указания его адреса
        if (client.ss_family == AF_INET) {
            struct sockaddr_in *a = (struct sockaddr_in *) &client; // если это IPv4, то мы приводим адрес клиента к типу struct sockaddr_in

            inet_ntop(AF_INET, &a->sin_addr, host, sizeof(host)); // преобразуем его IP адрес в строку и сохраняем её в host
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