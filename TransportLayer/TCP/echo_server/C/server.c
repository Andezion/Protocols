#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tcp.h"

int main(void) {
    uint16_t port = 12345;

    // tcp_socket_bind() создаёт SOCK_STREAM-сокет и привязывает его к порту
    // Никаких UDP-датаграмм - только настоящий TCP
    int s = tcp_socket_bind(port);
    if (s < 0) {
        return 1;
    }

    // listen() переводит сокет в пассивный режим
    // Ядро будет принимать входящие SYN и завершать рукопожатие самостоятельно,
    // складывая готовые соединения в очередь (backlog = 5)
    if (tcp_listen(s, 5) < 0) {
        close(s);
        return 1;
    }

    printf("Server listening on TCP port %u\n", port);

    struct sockaddr_in client;
    socklen_t clientlen = sizeof(client);

    // tcp_accept() блокируется до прихода соединения
    // Трёхстороннее рукопожатие уже завершено ядром - мы получаем
    // готовый дескриптор conn для обмена данными
    int conn = tcp_accept(s, (struct sockaddr *)&client, &clientlen);
    if (conn < 0) {
        close(s);
        return 1;
    }

    char host[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client.sin_addr, host, sizeof(host));
    printf("Accepted connection from %s:%u\n", host, ntohs(client.sin_port));

    // Читаем данные от клиента.
    char    buf[256];
    ssize_t n = tcp_recv(conn, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Received: %s", buf);

        // Отправляем ответ.
        const char *reply = "Hello from TCP server!\n";
        tcp_send(conn, reply, strlen(reply));
    }

    // Закрываем соединение - ядро выполнит четырёхстороннее завершение (FIN/ACK).
    close(conn);
    close(s);
    return 0;
}