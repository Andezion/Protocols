#include "udp.h"

#include <arpa/inet.h>  // для htons, htonl
#include <netinet/in.h> // для struct sockaddr_in, INADDR_ANY
#include <stddef.h>     // для size_t
#include <stdio.h>      // для perror
#include <string.h>     // для memset
#include <sys/socket.h> // для socket, bind, sendto, recvfrom
#include <unistd.h>     // для close

int udp_socket(void) {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("udp_socket: socket");
        return -1;
    }
    return s;
}

int udp_socket_bind(uint16_t port) {
    int s = udp_socket();
    if (s < 0) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("udp_socket_bind: bind");
        close(s);
        return -1;
    }

    return s;
}

