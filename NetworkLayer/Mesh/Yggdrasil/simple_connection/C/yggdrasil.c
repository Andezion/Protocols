#include "yggdrasil.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Отправляет ровно len байт, повторяя write() при частичной записи
static int send_all(int fd, const unsigned char *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = write(fd, buf + sent, len - sent);
        if (n <= 0) {
            return -1;
        }
        sent += (size_t) n;
    }
    return 0;
}

// Принимает ровно len байт, повторяя read() при частичном чтении
static int recv_all(int fd, unsigned char *buf, size_t len) {
    size_t got = 0;
    while (got < len) {
        ssize_t n = read(fd, buf + got, len - got);
        if (n <= 0) {
            return -1;
        }
        got += (size_t) n;
    }
    return 0;
}

// Порт AddrForKey() из src/address/address.go настоящего yggdrasil-go:
// адрес начинается с префикса 0x02, следующий байт - число единичных битов
// подряд в начале побитовой инверсии ключа, дальше - хвост инверсии ключа.
void ygg_addr_for_key(const unsigned char pub[crypto_sign_PUBLICKEYBYTES], unsigned char addr[YGG_ADDR_SIZE]) {
    unsigned char inv[crypto_sign_PUBLICKEYBYTES];
    for (size_t i = 0; i < sizeof(inv); i++) {
        inv[i] = (unsigned char) ~pub[i];
    }

    unsigned char tail[crypto_sign_PUBLICKEYBYTES];

    size_t taillen = 0;
    int done = 0;
    unsigned char ones = 0;
    unsigned char bits = 0;
    int nbits = 0;

    for (size_t idx = 0; idx < 8 * sizeof(inv); idx++) {
        int bit = (inv[idx / 8] & (0x80 >> (idx % 8))) >> (7 - (idx % 8));
        if (!done && bit != 0) {
            ones++;
            continue;
        }
        if (!done && bit == 0) {
            done = 1;
            continue;
        }
        bits = (unsigned char) ((bits << 1) | (unsigned char) bit);
        nbits++;
        if (nbits == 8) {
            nbits = 0;
            tail[taillen++] = bits;
        }
    }

    addr[0] = 0x02;
    addr[1] = ones;
    memcpy(addr + 2, tail, YGG_ADDR_SIZE - 2);
}

void ygg_addr_to_string(const unsigned char addr[YGG_ADDR_SIZE], char out[46]) {
    inet_ntop(AF_INET6, addr, out, 46);
}

int ygg_init(struct Yggdrasil *node) {
    if (sodium_init() < 0) return -1;

    if (crypto_sign_keypair(node->ed_pk, node->ed_sk) != 0) { 
        return -1;
    }
    if (crypto_sign_ed25519_sk_to_curve25519(node->box_sk, node->ed_sk) != 0) {
        return -1;
    }
    if (crypto_sign_ed25519_pk_to_curve25519(node->box_pk, node->ed_pk) != 0) {
        return -1;
    }

    ygg_addr_for_key(node->ed_pk, node->addr);
    return 0;
}

int ygg_send_metadata(int fd, const struct Yggdrasil *node) {
    unsigned char buf[YGG_META_SIZE];
    buf[0] = 'Y'; buf[1] = 'G'; buf[2] = 'G'; buf[3] = 'm';
    buf[4] = YGG_VERSION_MAJOR;
    buf[5] = YGG_VERSION_MINOR;
    memcpy(buf + 6, node->ed_pk, crypto_sign_PUBLICKEYBYTES);
    return send_all(fd, buf, sizeof(buf));
}

int ygg_recv_metadata(int fd, unsigned char peer_pub[crypto_sign_PUBLICKEYBYTES]) {
    unsigned char buf[YGG_META_SIZE];
    if (recv_all(fd, buf, sizeof(buf)) != 0) return -1;

    if (buf[0] != 'Y' || buf[1] != 'G' || buf[2] != 'G' || buf[3] != 'm') {
        fprintf(stderr, "ygg_recv_metadata: bad magic\n");
        return -1;
    }
    if (buf[4] != YGG_VERSION_MAJOR) {
        fprintf(stderr, "ygg_recv_metadata: incompatible major version %u\n", buf[4]);
        return -1;
    }

    memcpy(peer_pub, buf + 6, crypto_sign_PUBLICKEYBYTES);
    return 0;
}

int ygg_send_encrypted(int fd, const struct Yggdrasil *node, const unsigned char peer_box_pk[crypto_box_PUBLICKEYBYTES], const unsigned char *msg, size_t msglen) {
    size_t ciphertextlen = msglen + crypto_box_MACBYTES;
    size_t framelen = crypto_box_NONCEBYTES + ciphertextlen;
    if (framelen > 0xFFFF) {
        return -1;
    }

    unsigned char nonce[crypto_box_NONCEBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    unsigned char *frame = malloc(framelen);
    if (!frame) {
        return -1;
    }
    memcpy(frame, nonce, sizeof(nonce));

    int rc = crypto_box_easy(frame + sizeof(nonce), msg, msglen, nonce, peer_box_pk, node->box_sk);
    if (rc != 0) {
        free(frame);
        return -1;
    }

    unsigned char lenbuf[2];
    lenbuf[0] = (unsigned char) (framelen >> 8);
    lenbuf[1] = (unsigned char) (framelen & 0xFF);

    int ok = send_all(fd, lenbuf, sizeof(lenbuf)) == 0 && send_all(fd, frame, framelen) == 0;
    free(frame);
    return ok ? 0 : -1;
}

int ygg_recv_encrypted(int fd, const struct Yggdrasil *node, const unsigned char peer_box_pk[crypto_box_PUBLICKEYBYTES], unsigned char *out, size_t outcap, size_t *outlen) {
    unsigned char lenbuf[2];
    if (recv_all(fd, lenbuf, sizeof(lenbuf)) != 0) {
        return -1;
    }
    size_t framelen = ((size_t) lenbuf[0] << 8) | (size_t) lenbuf[1];

    if (framelen < crypto_box_NONCEBYTES + crypto_box_MACBYTES) {
        return -1;
    }

    unsigned char *frame = malloc(framelen);
    if (!frame) {
        return -1;
    }
    if (recv_all(fd, frame, framelen) != 0) {
        free(frame);
        return -1;
    }

    const unsigned char *nonce = frame;
    const unsigned char *ciphertext = frame + crypto_box_NONCEBYTES;
    size_t ciphertextlen = framelen - crypto_box_NONCEBYTES;
    size_t plainlen = ciphertextlen - crypto_box_MACBYTES;

    if (plainlen > outcap) {
        free(frame);
        return -1;
    }

    int rc = crypto_box_open_easy(out, ciphertext, ciphertextlen, nonce, peer_box_pk, node->box_sk);
    free(frame);
    if (rc != 0) {
        fprintf(stderr, "ygg_recv_encrypted: decryption/authentication failed\n");
        return -1;
    }

    *outlen = plainlen;
    return 0;
}
