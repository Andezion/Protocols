#include "tcp.h"

#include <arpa/inet.h>  // для htons, inet_ntop
#include <netinet/in.h> // для struct sockaddr_in, INADDR_ANY
#include <stddef.h>     // для size_t
#include <stdio.h>      // для perror
#include <string.h>     // для memset
#include <sys/socket.h> // для socket, bind, listen, accept, connect, send, recv, setsockopt
#include <unistd.h>     // для close

// Создаёт TCP-сокет (SOCK_STREAM) без привязки к порту
// SOCK_STREAM - потоковый сокет: данные передаются надёжно, в порядке отправки,
// без потерь и дублирования. Ядро само управляет SYN/ACK, повторами и окном
int tcp_socket(void) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("tcp_socket: socket");
        return -1;
    }

    // SO_REUSEADDR позволяет повторно занять порт сразу после закрытия сокета,
    // что удобно при частом перезапуске сервера во время разработки
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    return s;
}

// Создаёт TCP-сокет и привязывает его к указанному порту на всех интерфейсах
int tcp_socket_bind(uint16_t port) {
    int s = tcp_socket();
    if (s < 0) {
        return -1;
    }

    struct sockaddr_in addr; // структура для хранения адреса (IP + порт)
    memset(&addr, 0, sizeof(addr)); // обнуляем структуру перед заполнением
    addr.sin_family      = AF_INET; // семейство адресов - IPv4
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // принимаем соединения на всех сетевых интерфейсах
    addr.sin_port        = htons(port); // порт в сетевом порядке байт (big-endian)

    // Привязываем сокет к адресу и порту а если порт уже занят, bind() вернёт ошибку
    if (bind(s, (struct sockaddr *) & addr, sizeof(addr)) < 0) {
        perror("tcp_socket_bind: bind");
        close(s);
        return -1;
    }

    return s;
}

// Переводит сокет в пассивный режим: сокет начинает принимать входящие соединения
// backlog - размер очереди полностью установленных соединений, ожидающих accept()
int tcp_listen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        perror("tcp_listen: listen");
        return -1;
    }
    return 0;
}

// Принимает одно входящее TCP-соединение (блокируется до его прихода)
// Трёхстороннее рукопожатие (SYN -> SYN-ACK -> ACK) ядро завершает до возврата
// из accept(), поэтому мы получаем уже готовый к использованию дескриптор
int tcp_accept(int sockfd, struct sockaddr *clientaddr, socklen_t *addrlen) {
    int conn = accept(sockfd, clientaddr, addrlen);
    if (conn < 0) {
        perror("tcp_accept: accept");
        return -1;
    }
    return conn;
}

// Устанавливает TCP-соединение с сервером
// Под капотом ядро отправляет SYN, ждёт SYN-ACK и отвечает ACK -
// всё это происходит внутри connect(), нам вручную ничего делать не нужно
int tcp_connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen) {
    if (connect(sockfd, servaddr, addrlen) < 0) {
        perror("tcp_connect: connect");
        return -1;
    }
    return 0;
}

// Отправляет ровно len байт данных
// Один вызов send() может передать меньше байт, чем запрошено (при переполнении
// буфера ядра), поэтому повторяем отправку до полной передачи всех данных
ssize_t tcp_send(int sockfd, const void * data, size_t len) {
    size_t      sent = 0;
    const char *ptr  = (const char *) data;

    while (sent < len) {
        ssize_t n = send(sockfd, ptr + sent, len - sent, 0);
        if (n < 0) {
            perror("tcp_send: send");
            return -1;
        }
        sent += (size_t) n;
    }

    return (ssize_t) sent;
}

// Принимает до buflen байт данных из установленного соединения
// Возвращает 0, если сторона закрыла соединение (прислала FIN)
ssize_t tcp_recv(int sockfd, void * buf, size_t buflen) {
    ssize_t n = recv(sockfd, buf, buflen, 0);
    if (n < 0) {
        perror("tcp_recv: recv");
    }
    return n;
}
