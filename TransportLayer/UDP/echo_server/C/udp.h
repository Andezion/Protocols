#ifndef SIMPLE_UDP_H
#define SIMPLE_UDP_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>

#define UDP_MAX_PAYLOAD 65507 // Максимальный размер полезной нагрузки в UDP (65,535 - 8 байт заголовка - 20 байт IP-заголовка)

// Флаг который позволяет нескольким сокетам привязываться к одному порту, полезно для многопроцессных серверов
#define UDP_FLAG_REUSEADDR (1 << 0)
// Флаг который позволяет нескольким сокетам привязываться к одному порту и распределять входящие соединения между ними, полезно для многопроцессных серверов
#define UDP_FLAG_REUSEPORT (1 << 1)
// Флаг который устанавливает флаг FD_CLOEXEC на сокете, чтобы он автоматически закрывался при выполнении execve, полезно для предотвращения утечек дескрипторов в дочерних процессах
#define UDP_FLAG_CLOEXEC   (1 << 2)

int udp_socket(void);
int udp_socket_bind(uint16_t port);

int udp_socket_flags(int flags);
int udp_socket_bind_flags(uint16_t port, int flags);

ssize_t udp_sendto(int sockfd, const void *data, size_t len, const struct sockaddr *destaddr, socklen_t addrlen);
ssize_t udp_recvfrom(int sockfd, void *data, size_t len, struct sockaddr *srcaddr, socklen_t *addrlen);

#endif