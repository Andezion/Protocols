#ifndef SIMPLE_TCP_H
#define SIMPLE_TCP_H

#include <stdint.h> // для uint16_t, uint32_t и uint8_t
#include <stddef.h> // для size_t
#include <sys/types.h> // для ssize_t
#include <sys/socket.h> // для struct sockaddr и socklen_t

#define TCP_FLAG_FIN 0x01 // флаг для завершения соединения
#define TCP_FLAG_SYN 0x02 // флаг для установления соединения, используется при открытии соединения, чтобы синхронизировать начальные номера последовательности (ISN) между клиентом и сервером
#define TCP_FLAG_RST 0x04 // флаг для сброса соединения, используется для немедленного закрытия соединения и уведомления другой стороны о проблеме
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20

// функция для того чтобы "открыть" сокет и занять его
int udp_socket_bind(uint16_t port); // передаём порт, а взвращаем дескриптор сокета

int tcp_send_packet(int sockfd, const struct sockaddr *dest, socklen_t destlen,
                    uint32_t seq, uint32_t ack, uint8_t flags,
                    const void *data, size_t len);

ssize_t tcp_recv_packet(int sockfd, struct sockaddr *src, socklen_t *srclen,
                        uint32_t *seq, uint32_t *ack, uint8_t *flags,
                        void *buf, size_t buflen, int timeout_ms);

int tcp_connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen,
                uint32_t *out_isn);

int tcp_accept(int sockfd, struct sockaddr *clientaddr, socklen_t *addrlen,
               uint32_t *client_isn, uint32_t *server_isn);

#endif 


