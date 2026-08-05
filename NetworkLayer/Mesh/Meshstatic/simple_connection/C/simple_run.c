#include "meshstatic.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void aes_128_ctr_encrypt(const uint8_t *key, const uint8_t *nonce, const uint8_t *input, size_t len, uint8_t *output) {
    // упрощённый потоковый шифр вместо настоящего AES - но он симметричен,
    // а значит подходит для проверки логики CTR-режима (шифрование == расшифровка)
    for (size_t i = 0; i < len; i++) {
        output[i] = input[i] ^ key[i % 16] ^ nonce[i % 16];
    }
}

uint32_t esp32_get_node_id(void) { return 0xABCDEFFF; }
uint32_t esp32_random_u32(void) { return 0x12345678; }

static uint64_t read_varint(const uint8_t *in, size_t *pos) {
    uint64_t result = 0;
    int shift = 0;
    uint8_t byte;
    do {
        byte = in[(*pos)++];
        result |= (uint64_t)(byte & 0x7F) << shift;
        shift += 7;
    } while (byte & 0x80);
    return result;
}

static meshtastic_MeshPacket parse_wire_packet(const uint8_t *buf, size_t len) {
    static uint8_t encrypted_storage[256];
    meshtastic_MeshPacket packet = {0};
    size_t pos = 0;

    while (pos < len) {
        uint64_t tag = read_varint(buf, &pos);
        uint32_t field = (uint32_t)(tag >> 3);
        uint8_t wire_type = (uint8_t)(tag & 0x7);

        if (wire_type == 0) {
            uint64_t value = read_varint(buf, &pos);
            switch (field) {
                case 1: packet.to = (uint32_t)value; break;
                case 2: packet.from = (uint32_t)value; break;
                case 6: packet.id = (uint32_t)value; break;
                case 9: packet.hop_limit = (uint32_t)value; break;
                default: break;
            }
        } else {
            uint64_t flen = read_varint(buf, &pos);
            if (field == 5) {
                memcpy(encrypted_storage, buf + pos, flen);
                packet.encrypted.bytes = encrypted_storage;
                packet.encrypted.size = flen;
            }
            pos += flen;
        }
    }

    return packet;
}

void sx1262_send_packet(const uint8_t *buffer, size_t len) {
    printf("[radio] отправлено %zu байт эфира\n", len);

    // имитируем приём того же радиопакета на другом узле
    meshtastic_MeshPacket received_packet = parse_wire_packet(buffer, len);
    meshtastic_Data received_data = receive_mesh_packet(&received_packet);
    const char *text = handle_data(received_data);

    printf("[receiver] расшифрованный текст: %s\n", text ? text : "(нет)");
    assert(text != NULL);
    assert(strcmp(text, "hello mesh") == 0);
}

int main(void) {
    send_mesh_text_message("hello mesh");
    printf("OK: round-trip send -> radio -> receive -> decode успешен\n");
    return 0;
}
