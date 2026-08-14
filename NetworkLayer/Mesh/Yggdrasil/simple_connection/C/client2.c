#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "yggdrasil.h"

// client2 - дозванивающийся узел 
int main(void) {
    struct Yggdrasil node;
    if (ygg_init(&node) != 0) {
        fprintf(stderr, "ygg_init failed\n");
        return 1;
    }

    char selfaddr[46];
    ygg_addr_to_string(node.addr, selfaddr);
    printf("[client2] identity ready, my Yggdrasil address: %s\n", selfaddr);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { 
        perror("socket"); 
        return 1; 
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9001);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    printf("[client2] connecting to client1 on 127.0.0.1:9001...\n");
    if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) { 
        perror("connect"); 
        return 1; 
    }

    printf("[client2] connected, exchanging metadata...\n");

    unsigned char peer_ed_pk[crypto_sign_PUBLICKEYBYTES];
    if (ygg_recv_metadata(s, peer_ed_pk) != 0) { 
        fprintf(stderr, "recv metadata failed\n"); 
        return 1; 
    }

    if (ygg_send_metadata(s, &node) != 0) { 
        fprintf(stderr, "send metadata failed\n"); 
        return 1; 
    }

    unsigned char peer_addr[YGG_ADDR_SIZE];
    ygg_addr_for_key(peer_ed_pk, peer_addr);
    char peeraddrstr[46];
    ygg_addr_to_string(peer_addr, peeraddrstr);
    printf("[client2] peer identity confirmed, peer Yggdrasil address: %s\n", peeraddrstr);

    unsigned char peer_box_pk[crypto_box_PUBLICKEYBYTES];
    if (crypto_sign_ed25519_pk_to_curve25519(peer_box_pk, peer_ed_pk) != 0) {
        fprintf(stderr, "pk_to_curve25519 failed\n");
        return 1;
    }

    unsigned char msg[256];
    size_t msglen = 0;
    if (ygg_recv_encrypted(s, &node, peer_box_pk, msg, sizeof(msg) - 1, &msglen) != 0) {
        fprintf(stderr, "recv encrypted failed\n");
        return 1;
    }
    msg[msglen] = '\0';
    printf("[client2] received decrypted: %s\n", msg);

    const char *reply = "hello back from client2, this reached you encrypted too";
    if (ygg_send_encrypted(s, &node, peer_box_pk, (const unsigned char *) reply, strlen(reply)) != 0) {
        fprintf(stderr, "send encrypted failed\n");
        return 1;
    }
    printf("[client2] sent encrypted: %s\n", reply);

    close(s);
    return 0;
}
