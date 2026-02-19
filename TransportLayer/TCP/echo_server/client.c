#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../tcp.h"

int main(void) {
    const char *server_ip   = "127.0.0.1";
    uint16_t    server_port = 12345;

    // tcp_socket() создаёт SOCK_STREAM-сокет — настоящий TCP-сокет,
    // а не SOCK_DGRAM (UDP), как было раньше.
    int s = tcp_socket();
    if (s < 0) {
        return 1;
    }

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port   = htons(server_port);
    inet_pton(AF_INET, server_ip, &serv.sin_addr);

    // tcp_connect() вызывает системный connect().
    // Ядро само выполнит трёхстороннее рукопожатие:
    //   клиент  →  SYN           →  сервер
    //   клиент  ←  SYN-ACK       ←  сервер
    //   клиент  →  ACK           →  сервер
    // После возврата из connect() соединение уже установлено.
    if (tcp_connect(s, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        printf("Connection failed.\n");
        close(s);
        return 1;
    }

    printf("Connected to %s:%u\n", server_ip, server_port);

    // Отправляем данные — ядро разобьёт их на TCP-сегменты,
    // добавит заголовки, порядковые номера и будет следить за доставкой.
    const char *msg = "Hello from TCP client!\n";
    if (tcp_send(s, msg, strlen(msg)) < 0) {
        printf("Send failed.\n");
        close(s);
        return 1;
    }

    // Читаем ответ сервера.
    char    buf[256];
    ssize_t n = tcp_recv(s, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Server replied: %s", buf);
    } else if (n == 0) {
        printf("Server closed the connection.\n");
    }

    // close() инициирует четырёхстороннее закрытие:
    //   клиент - FIN - сервер
    //   клиент - ACK - сервер
    //   клиент  ←  FIN           ←  сервер
    //   клиент  →  ACK           →  сервер
    close(s);
    return 0;
}
