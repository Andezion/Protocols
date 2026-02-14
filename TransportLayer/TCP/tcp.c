#include "tcp.h"

#include <arpa/inet.h> // для inet_ntop и ntohs
#include <netinet/in.h> // для struct sockaddr_in
#include <stddef.h> // для size_t
#include <stdio.h> // для printf и perror 
#include <stdlib.h> // для exit
#include <string.h> // для memset и memcpy
#include <sys/select.h> // для select
#include <sys/socket.h> // для socket, bind, sendmsg, recvfrom и struct msghdr
#include <sys/time.h> // для gettimeofday
#include <unistd.h> // для close

// наша структура заголовка TCP-подобного протокола, упакованная для предотвращения добавления компилятором выравнивания между полями
struct __attribute__((packed)) simple_tcp_hdr {
    uint16_t src_port; // куда шлём
    uint16_t dst_port; // откуда пришло
    uint32_t seq; // номер нашей последовательности
    uint32_t ack; // номер подтверждения, который мы ожидаем от другой стороны
    uint8_t  flags; // наши флаги 
    uint8_t  _pad; // наше место свободных байтов
    uint16_t window; // размер окна, смотри схему
};

int udp_socket_bind(uint16_t port) {
    int s = socket(AF_INET, SOCK_DGRAM, 0); // создаём UDP сокет, который мы будем использовать для имитации TCP
    if (s < 0) { // может не получится если порт уже занят или по другим причинам, тогда возвращаем -1
        return -1;
    }

    struct sockaddr_in addr = {0}; // просто создание структуры 

    addr.sin_family = AF_INET; // мы используем IPv4
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // мы будем принимать пакеты, адресованные любому из наших IP-адресов
    addr.sin_port = htons(port); // мы будем слушать на этом порту

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) { // пытаемся забить сокет на нужный порт
        close(s); // может не получится если порт уже занят или по другим причинам, тогда закрываем сокет и возвращаем -1
        return -1;
    }

    return s;
}

int tcp_send_packet(int sockfd, const struct sockaddr *dest, socklen_t destlen,
                    uint32_t seq, uint32_t ack, uint8_t flags,
                    const void *data, size_t len) {
    struct simple_tcp_hdr h; // создаем пакет наш
    memset(&h, 0, sizeof(h)); // выделяем память для заголовка и обнуляем её

    h.src_port = 0; // поменять!
    h.dst_port = 0; // поменять! 
    h.seq = htonl(seq); // номер последовательности, который мы хотим отправить, преобразуем его в сетевой порядок байтов
    h.ack = htonl(ack); // номер подтверждения, который мы ожидаем от другой стороны, преобразуем его в сетевой порядок байтов
    h.flags = flags; // наши флаги 
    h.window = htons(0); // наше окно

    struct iovec iov[2]; // структура для описания вектора буферов, который мы будем отправлять, она позволяет нам отправлять заголовок и данные в одном вызове sendmsg без необходимости копировать их в один буфер
    iov[0].iov_base = &h; // первый элемент вектора указывает на наш заголовок
    iov[0].iov_len = sizeof(h); // длина нашего заголовка
    iov[1].iov_base = (void *)data; // второй элемент вектора указывает на наши данные, которые мы хотим отправить
    iov[1].iov_len = len; // длина наших данных

    struct msghdr msg = { 0 }; // структура для описания сообщения, которое мы будем отправлять, она позволяет нам указать адрес назначения, вектор буферов и другие параметры отправки
    msg.msg_name = (void *) dest; // куда шлём
    msg.msg_namelen = destlen; // длина адреса назначения
    msg.msg_iov = iov; // указатель на вектор буферов, который мы хотим отправить
    msg.msg_iovlen = (len > 0 ? 2 : 1); // количество элементов в векторе буферов, если у нас есть данные для отправки, то мы отправляем и заголовок и данные, иначе мы отправляем только заголовок

    ssize_t sent = sendmsg(sockfd, &msg, 0); // отправляем наше сообщение, если отправка прошла успешно, то sendmsg вернет количество байт, которые были отправлены, иначе он вернет -1 и установит errno
    return (sent < 0) ? -1 : 0;
}

ssize_t tcp_recv_packet(int sockfd, struct sockaddr *src, socklen_t *srclen,
                        uint32_t *seq, uint32_t *ack, uint8_t *flags,
                        void *buf, size_t buflen, int timeout_ms) {
    fd_set rfds; // множество дескрипторов для функции select, которая позволяет нам ждать, пока сокет станет готовым для чтения, с возможностью указать таймаут, чтобы мы не ждали вечно, если ничего не придет

    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    struct timeval tv;
    struct timeval *tvp = NULL;
    
    if (timeout_ms >= 0) {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        tvp = &tv;
    }

    int rv = select(sockfd + 1, &rfds, NULL, NULL, tvp);
    if (rv <= 0) {
        return -1;
    }

    unsigned char tmp[65536];
    socklen_t len = *srclen;

    ssize_t r = recvfrom(sockfd, tmp, sizeof(tmp), 0, src, &len);
    if (r <= 0) {
        return -1;
    }

    if ((size_t)r < sizeof(struct simple_tcp_hdr)) {
        return -1;
    }

    struct simple_tcp_hdr hdr;
    memcpy(&hdr, tmp, sizeof(hdr));

    *seq = ntohl(hdr.seq);
    *ack = ntohl(hdr.ack);

    *flags = hdr.flags;

    size_t payload = r - sizeof(hdr);
    if (payload && buf && buflen) {
        size_t tocopy = (payload < buflen) ? payload : buflen;
        memcpy(buf, tmp + sizeof(hdr), tocopy);
    }

    *srclen = len;
    return (ssize_t) payload;
}

static uint32_t gen_isn(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t) ((tv.tv_sec ^ tv.tv_usec) & 0xffffffff);
}

int tcp_connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen,
                uint32_t *out_isn) {
    uint32_t isn = gen_isn();
    
    if (tcp_send_packet(sockfd, servaddr, addrlen, isn, 0, TCP_FLAG_SYN, NULL, 0) < 0)
        return -1;

    struct sockaddr_storage src;
    socklen_t srclen = sizeof(src);

    uint32_t seq, ack;
    uint8_t flags;

    if (tcp_recv_packet(sockfd, (struct sockaddr *) &src, &srclen, &seq, &ack, &flags, NULL, 0, 5000) < 0) {
        return -1;
    }

    if (!(flags & TCP_FLAG_SYN) || !(flags & TCP_FLAG_ACK)) {
        return -1;
    }

    if (ack != isn + 1) { 
        return -1;
    }

    uint32_t server_isn = seq;
    if (tcp_send_packet(sockfd, servaddr, addrlen, isn+1, server_isn + 1, TCP_FLAG_ACK, NULL, 0) < 0) {
        return -1;
    }

    if (out_isn) {
        *out_isn = isn;
    }

    return 0;
}

int tcp_accept(int sockfd, struct sockaddr *clientaddr, socklen_t *addrlen,
               uint32_t *client_isn, uint32_t *server_isn) {
    struct sockaddr_storage src;
    socklen_t srclen = sizeof(src);

    uint32_t seq, ack;
    uint8_t flags;

    if (tcp_recv_packet(sockfd, (struct sockaddr *) &src, &srclen, &seq, &ack, &flags, NULL, 0, -1) < 0) {
        return -1;
    }

    if (!(flags & TCP_FLAG_SYN)) { 
        return -1;
    }

    if (clientaddr && addrlen) {
        if (*addrlen >= srclen) {
            memcpy(clientaddr, &src, srclen);
            *addrlen = srclen;
        }
    }

    uint32_t c_isn = seq;
    uint32_t s_isn = gen_isn();

    if (tcp_send_packet(sockfd, (struct sockaddr *) &src, srclen, s_isn, c_isn + 1, TCP_FLAG_SYN|TCP_FLAG_ACK, NULL, 0) < 0) {
        return -1;
    }

    srclen = sizeof(src);

    if (tcp_recv_packet(sockfd, (struct sockaddr *) &src, &srclen, &seq, &ack, &flags, NULL, 0, 5000) < 0) {
        return -1;
    }

    if (!(flags & TCP_FLAG_ACK)) {
        return -1;
    }

    if (ack != s_isn + 1) {
        return -1;
    }

    if (client_isn) {
        *client_isn = c_isn;
    }

    if (server_isn) {
        *server_isn = s_isn;
    }

    return 0;
}
