#ifndef SIMPLE_UDP_H
#define SIMPLE_UDP_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>

#define UDP_MAX_PAYLOAD 65507 // Максимальный размер полезной нагрузки в UDP (65,535 - 8 байт заголовка - 20 байт IP-заголовка)

int udp_socket(void);
int udp_socket_bind(uint16_t port);
ssize_t udp_sendto(int sockfd, const void *data, size_t len, const struct sockaddr *destaddr, socklen_t addrlen);
ssize_t udp_recvfrom(int sockfd, void *data, size_t len, struct sockaddr *srcaddr, socklen_t *addrlen);

#endif