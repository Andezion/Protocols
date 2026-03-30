#include "udp.h"

#include <arpa/inet.h>  // для htons, htonl
#include <netinet/in.h> // для struct sockaddr_in, INADDR_ANY
#include <stddef.h>     // для size_t
#include <fcntl.h>
#include <stdio.h>      // для perror
#include <string.h>     // для memset
#include <sys/socket.h> // для socket, bind, sendto, recvfrom
#include <unistd.h>     // для close
int udp_socket_flags(int flags)
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("udp_socket: socket");
        return -1;
    }

    if (flags & UDP_FLAG_REUSEADDR) {
        int opt = 1;
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            perror("udp_socket: setsockopt SO_REUSEADDR");
        }
    }

    if (flags & UDP_FLAG_REUSEPORT) {
    #ifdef SO_REUSEPORT
        int opt = 1;
        if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            perror("udp_socket: setsockopt SO_REUSEPORT");
        }
    #else
        fprintf(stderr, "udp_socket: SO_REUSEPORT not supported on this platform\n");
    #endif
    }

    if (flags & UDP_FLAG_CLOEXEC) {
        int fdflags = fcntl(s, F_GETFD);
        if (fdflags < 0) {
            perror("udp_socket: fcntl F_GETFD");
        } else {
            if (fcntl(s, F_SETFD, fdflags | FD_CLOEXEC) < 0) {
                perror("udp_socket: fcntl F_SETFD FD_CLOEXEC");
            }
        }
    }

    return s;
}

/* Backwards-compatible wrapper: previous behavior enabled SO_REUSEADDR by default */
int udp_socket(void)
{
    return udp_socket_flags(UDP_FLAG_REUSEADDR);
}

int udp_socket_bind_flags(uint16_t port, int flags)
{
    int s = udp_socket_flags(flags);
    if (s < 0) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("udp_socket_bind: bind");
        close(s);
        return -1;
    }

    return s;
}

/* Backwards-compatible wrapper: preserve previous default (REUSEADDR) */
int udp_socket_bind(uint16_t port)
{
    return udp_socket_bind_flags(port, UDP_FLAG_REUSEADDR);
}

ssize_t udp_sendto(int sockfd, const void *data, size_t len, const struct sockaddr *destaddr, socklen_t addrlen)
{
    ssize_t sent = sendto(sockfd, data, len, 0, destaddr, addrlen);
    if (sent < 0) {
        perror("udp_sendto: sendto");
    }
    return sent;
}

ssize_t udp_recvfrom(int sockfd, void *data, size_t len, struct sockaddr *srcaddr, socklen_t *addrlen)
{
    ssize_t received = recvfrom(sockfd, data, len, 0, srcaddr, addrlen);
    if (received < 0) {
        perror("udp_recvfrom: recvfrom");
    }
    return received;
}