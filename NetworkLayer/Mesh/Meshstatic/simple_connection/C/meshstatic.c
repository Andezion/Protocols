#include "meshstatic.h"
#include <string.h>

// https://github.com/meshtastic/protobufs

#define PB_WIRETYPE_VARINT 0
#define PB_WIRETYPE_LEN    2

static size_t pb_write_varint(uint8_t *out, uint64_t value) {
    size_t n = 0;
    do {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if (value) byte |= 0x80;
        out[n++] = byte;
    } while (value);
    return n;
}

static size_t pb_write_tag(uint8_t *out, uint32_t field_number, uint8_t wire_type) {
    return pb_write_varint(out, ((uint64_t)field_number << 3) | wire_type);
}

// proto3 не сериализует скалярные поля со значением по умолчанию (0)
static size_t pb_write_uint32_field(uint8_t *out, uint32_t field_number, uint32_t value) {
    if (value == 0) return 0;
    size_t n = pb_write_tag(out, field_number, PB_WIRETYPE_VARINT);
    n += pb_write_varint(out + n, value);
    return n;
}

static size_t pb_write_bytes_field(uint8_t *out, uint32_t field_number, const uint8_t *data, size_t len) {
    size_t n = pb_write_tag(out, field_number, PB_WIRETYPE_LEN);
    n += pb_write_varint(out + n, len);
    memcpy(out + n, data, len);
    n += len;
    return n;
}

void send_mesh_text_message(const char* text) {
    // meshtastic.Data: portnum = 1, payload = 2
    uint8_t serialized_data[256];
    size_t data_len = 0;
    data_len += pb_write_uint32_field(serialized_data + data_len, 1, 1 /* TEXT_MESSAGE_APP */);
    data_len += pb_write_bytes_field(serialized_data + data_len, 2, (const uint8_t*)text, strlen(text));

    // шифрование (AES-128-CTR)
    // в Meshtastic дефолтный ключ для "Channel 0" общеизвестен -
    // d4f1bb3a20290759f0bcffab2b6024a9 (AQ== в base64)
    uint8_t default_key[16] = {0xd4, 0xf1, 0xbb, 0x3a, 0x20, 0x29, 0x07, 0x59,
                               0xf0, 0xbc, 0xff, 0xab, 0x2b, 0x60, 0x24, 0xa9};
    uint8_t encrypted_data[256];
    uint32_t packet_id = 0x12345678; // уникальный ID этого пакета
    uint32_t from_node = 0xABCDEFFF; // наш NodeID (вычисляется из MAC-адреса ESP32)

    // формируем Nonce (IV) для AES-CTR по стандарту Meshtastic -
    // PacketID (4 байта) + FromNode (4 байта) + 8 байт нулей
    uint8_t nonce[16] = {0};
    memcpy(nonce, &packet_id, 4);
    memcpy(nonce + 4, &from_node, 4);

    // шифруем данные из serialized_data в encrypted_data
    aes_128_ctr_encrypt(default_key, nonce, serialized_data, data_len, encrypted_data);

    // meshtastic.MeshPacket: to = 1, from = 2, encrypted = 5, id = 6, hop_limit = 9
    uint8_t final_radio_buffer[384];
    size_t final_len = 0;
    final_len += pb_write_uint32_field(final_radio_buffer + final_len, 1, 0xFFFFFFFF); // бродкаст (всем узлам)
    final_len += pb_write_uint32_field(final_radio_buffer + final_len, 2, from_node);
    final_len += pb_write_bytes_field(final_radio_buffer + final_len, 5, encrypted_data, data_len);
    final_len += pb_write_uint32_field(final_radio_buffer + final_len, 6, packet_id);
    final_len += pb_write_uint32_field(final_radio_buffer + final_len, 9, 3); // пакет переродится максимум 3 раза через репитеры

    // отправка в радиочип LoRa
    // теперь final_radio_buffer содержит "чистый" байт-код протокола Meshtastic
    // передаем его в драйвер трансивера (например, SX1262) по SPI
    sx1262_send_packet(final_radio_buffer, final_len);
}
