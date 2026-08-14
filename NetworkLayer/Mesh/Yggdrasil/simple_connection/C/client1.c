#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "yggdrasil.h"

// client1 - принимающий узел, сначала слушаем, потом кто-то к нам приходит
int main(void) {
    struct Yggdrasil node;
    if (ygg_init(&node) != 0) {
        fprintf(stderr, "ygg_init failed\n");
        return 1;
    }

    char selfaddr[46];
    ygg_addr_to_string(node.addr, selfaddr);
    printf("[client1] identity ready, my Yggdrasil address: %s\n", selfaddr);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { 
        perror("socket"); 
        return 1; 
    }

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9001);

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) { 
        perror("bind"); 
        return 1; 
    }

    if (listen(s, 1) < 0) { 
        perror("listen"); 
        return 1; 
    }

    printf("[client1] waiting for a peer on TCP port 9001...\n");

    int conn = accept(s, NULL, NULL);
    if (conn < 0) { 
        perror("accept"); 
        return 1; 
    }

    printf("[client1] peer connected, exchanging metadata...\n");

    if (ygg_send_metadata(conn, &node) != 0) { 
        fprintf(stderr, "send metadata failed\n"); 
        return 1; 
    }

    unsigned char peer_ed_pk[crypto_sign_PUBLICKEYBYTES];
    if (ygg_recv_metadata(conn, peer_ed_pk) != 0) { 
        fprintf(stderr, "recv metadata failed\n"); 
        return 1; 
    }

    unsigned char peer_addr[YGG_ADDR_SIZE];
    ygg_addr_for_key(peer_ed_pk, peer_addr);
    char peeraddrstr[46];
    ygg_addr_to_string(peer_addr, peeraddrstr);

    printf("[client1] peer identity confirmed, peer Yggdrasil address: %s\n", peeraddrstr);

    unsigned char peer_box_pk[crypto_box_PUBLICKEYBYTES];
    if (crypto_sign_ed25519_pk_to_curve25519(peer_box_pk, peer_ed_pk) != 0) {
        fprintf(stderr, "pk_to_curve25519 failed\n");
        return 1;
    }

    const char *hello = "hello from client1, over an encrypted link";
    if (ygg_send_encrypted(conn, &node, peer_box_pk, (const unsigned char *) hello, strlen(hello)) != 0) {
        fprintf(stderr, "send encrypted failed\n");
        return 1;
    }
    printf("[client1] sent encrypted: %s\n", hello);

    unsigned char reply[256];
    size_t replylen = 0;

    if (ygg_recv_encrypted(conn, &node, peer_box_pk, reply, sizeof(reply) - 1, &replylen) != 0) {
        fprintf(stderr, "recv encrypted failed\n");
        return 1;
    }
    reply[replylen] = '\0';
    
    printf("[client1] received decrypted: %s\n", reply);

    close(conn);
    close(s);
    return 0;
}
