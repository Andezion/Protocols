#pragma once

#include <stddef.h>
#include <sodium.h>

#define YGG_VERSION_MAJOR 0
#define YGG_VERSION_MINOR 1

#define YGG_ADDR_SIZE 16
// magic(4) + major(1) + minor(1) + ed25519 pubkey(32)
#define YGG_META_SIZE (4 + 1 + 1 + crypto_sign_PUBLICKEYBYTES)

// identity узла - ключи Ed25519 (подписи/личность) и производные от них ключи
// Curve25519 (используются для NaCl box), плюс адрес, выведенный из pubkey
struct Yggdrasil {
    unsigned char ed_pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char ed_sk[crypto_sign_SECRETKEYBYTES];
    unsigned char box_pk[crypto_box_PUBLICKEYBYTES];
    unsigned char box_sk[crypto_box_SECRETKEYBYTES];
    unsigned char addr[YGG_ADDR_SIZE];
};

// генерирует пару Ed25519-ключей, выводит из неё пару Curve25519-ключей и
// crypto-адрес, возвращает 0 при успехе
int ygg_init(struct Yggdrasil *node);

// выводит IPv6-адрес 200::/7 из публичного Ed25519-ключа
void ygg_addr_for_key(const unsigned char pub[crypto_sign_PUBLICKEYBYTES],
                       unsigned char addr[YGG_ADDR_SIZE]);

// форматирует адрес узла в обычную IPv6-строку вида "201:85c5:..."
void ygg_addr_to_string(const unsigned char addr[YGG_ADDR_SIZE], char out[46]);

// Отправляет/принимает по сокету пакет metadata (версия + Ed25519-identity)
int ygg_send_metadata(int fd, const struct Yggdrasil *node);
int ygg_recv_metadata(int fd, unsigned char peer_pub[crypto_sign_PUBLICKEYBYTES]);

// шифрует сообщение для пира через NaCl box (X25519 + XSalsa20-Poly1305) и
// отправляет кадром [uint16 длина][nonce 24 байта][шифротекст]
int ygg_send_encrypted(int fd, const struct Yggdrasil *node,
                        const unsigned char peer_box_pk[crypto_box_PUBLICKEYBYTES],
                        const unsigned char *msg, size_t msglen);

// принимает и расшифровывает такой кадр, out должен вмещать хотя бы outcap байт
int ygg_recv_encrypted(int fd, const struct Yggdrasil *node,
                        const unsigned char peer_box_pk[crypto_box_PUBLICKEYBYTES],
                        unsigned char *out, size_t outcap, size_t *outlen);
