#include "meshstatic.h"
#include <string.h>

// https://github.com/meshtastic/protobufs

#define PB_WIRETYPE_VARINT 0
#define PB_WIRETYPE_LEN    2

// В Meshtastic дефолтный ключ для "Channel 0" общеизвестен - d4f1bb3a20290759f0bcffab2b6024a9 (AQ== в base64)
static const uint8_t DEFAULT_CHANNEL_KEY[16] = {
    0xd4, 0xf1, 0xbb, 0x3a, 0x20, 0x29, 0x07, 0x59,
    0xf0, 0xbc, 0xff, 0xab, 0x2b, 0x60, 0x24, 0xa9
};

// Ограничиваем длину текста заранее, чтобы сериализация гарантированно влезала
// в 256-байтовые буферы ниже (иначе длинное сообщение переполнило бы их)
#define MESHSTATIC_MAX_PAYLOAD_LEN 200

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
    if (len == 0) return 0;
    size_t n = pb_write_tag(out, field_number, PB_WIRETYPE_LEN);
    n += pb_write_varint(out + n, len);
    memcpy(out + n, data, len);
    n += len;
    return n;
}

static size_t pb_read_varint(const uint8_t *in, size_t in_len, uint64_t *value) {
    uint64_t result = 0;
    size_t n = 0;
    int shift = 0;
    while (n < in_len) {
        uint8_t byte = in[n++];
        result |= (uint64_t)(byte & 0x7F) << shift;
        if (!(byte & 0x80)) break;
        shift += 7;
    }
    *value = result;
    return n;
}

typedef void (*pb_field_cb)(uint32_t field_number, const uint8_t *bytes, size_t len, uint64_t varint, void *ctx);

// проходит по верхнеуровневым protobuf-полям буфера, вызывая cb на каждое найденное поле
static void pb_parse_fields(const uint8_t *in, size_t in_len, pb_field_cb cb, void *ctx) {
    size_t pos = 0;
    while (pos < in_len) {
        uint64_t tag;
        size_t n = pb_read_varint(in + pos, in_len - pos, &tag);
        if (n == 0) return;
        pos += n;

        uint32_t field_number = (uint32_t)(tag >> 3);
        uint8_t wire_type = (uint8_t)(tag & 0x7);

        if (wire_type == PB_WIRETYPE_VARINT) {
            uint64_t value;
            n = pb_read_varint(in + pos, in_len - pos, &value);
            if (n == 0) return;
            pos += n;
            cb(field_number, NULL, 0, value, ctx);
        } else if (wire_type == PB_WIRETYPE_LEN) {
            uint64_t len;
            n = pb_read_varint(in + pos, in_len - pos, &len);
            if (n == 0 || pos + n + len > in_len) return;
            pos += n;
            cb(field_number, in + pos, (size_t)len, 0, ctx);
            pos += (size_t)len;
        } else {
            return; // неподдерживаемый wire type тут не встречается
        }
    }
}

// meshtastic.Data: portnum = 1, payload = 2, id = 3, want_response = 4
static size_t serialize_data(const meshtastic_Data *data, uint8_t *out) {
    size_t n = 0;
    n += pb_write_uint32_field(out + n, 1, data->portnum);
    n += pb_write_bytes_field(out + n, 2, data->payload.bytes, data->payload.size);
    n += pb_write_uint32_field(out + n, 3, data->id);
    if (data->want_response) {
        n += pb_write_uint32_field(out + n, 4, 1);
    }
    return n;
}

static void deserialize_data_field(uint32_t field_number, const uint8_t *bytes, size_t len, uint64_t varint, void *ctx) {
    meshtastic_Data *data = (meshtastic_Data *)ctx;
    switch (field_number) {
        case 1: data->portnum = (uint32_t)varint; break;
        case 2: data->payload.bytes = bytes; data->payload.size = len; break;
        case 3: data->id = (uint32_t)varint; break;
        case 4: data->want_response = (varint != 0); break;
        default: break; // неизвестные поля пропускаем
    }
}

static meshtastic_Data deserialize_data(const uint8_t *in, size_t in_len) {
    meshtastic_Data data = {0};
    pb_parse_fields(in, in_len, deserialize_data_field, &data);
    return data;
}

// meshtastic.MeshPacket: to = 1, from = 2, encrypted = 5, id = 6, hop_limit = 9
static size_t serialize_mesh_packet(const meshtastic_MeshPacket *packet, uint8_t *out) {
    size_t n = 0;
    n += pb_write_uint32_field(out + n, 1, packet->to);
    n += pb_write_uint32_field(out + n, 2, packet->from);
    n += pb_write_bytes_field(out + n, 5, packet->encrypted.bytes, packet->encrypted.size);
    n += pb_write_uint32_field(out + n, 6, packet->id);
    n += pb_write_uint32_field(out + n, 9, packet->hop_limit);
    return n;
}

// нонс для AES-CTR
// Nonce (IV) для AES-CTR по стандарту Meshtastic - PacketID (4 байта) + FromNode (4 байта) + 8 байт нулей
static void build_nonce(uint8_t nonce[16], uint32_t packet_id, uint32_t from_node) {
    memset(nonce, 0, 16);
    memcpy(nonce, &packet_id, 4);
    memcpy(nonce + 4, &from_node, 4);
}

meshtastic_Data send_data(const char *text) {
    meshtastic_Data data = {0};
    data.portnum = MESHTASTIC_PORTNUM_TEXT_MESSAGE_APP;

    size_t len = strlen(text);
    if (len > MESHSTATIC_MAX_PAYLOAD_LEN) len = MESHSTATIC_MAX_PAYLOAD_LEN;

    data.payload.bytes = (const uint8_t *)text;
    data.payload.size = len;
    data.id = esp32_random_u32();
    data.want_response = false;
    return data;
}

meshtastic_MeshPacket send_mesh_packet(meshtastic_MeshPacket *packet) {
    // на входе packet->encrypted указывает на ещё не зашифрованный сериализованный Data -
    // шифруем его на месте и отправляем итоговый пакет по радиоканалу
    uint8_t nonce[16];
    build_nonce(nonce, packet->id, packet->from);

    static uint8_t encrypted_buf[256];
    size_t len = packet->encrypted.size;
    if (len > sizeof(encrypted_buf)) len = sizeof(encrypted_buf);

    aes_128_ctr_encrypt(DEFAULT_CHANNEL_KEY, nonce, packet->encrypted.bytes, len, encrypted_buf);
    packet->encrypted.bytes = encrypted_buf;
    packet->encrypted.size = len;

    uint8_t wire_buffer[384];
    size_t wire_len = serialize_mesh_packet(packet, wire_buffer);

    sx1262_send_packet(wire_buffer, wire_len);

    return *packet;
}

meshtastic_Data receive_mesh_packet(meshtastic_MeshPacket *packet) {
    uint8_t nonce[16];
    build_nonce(nonce, packet->id, packet->from);

    static uint8_t decrypted_buf[256];
    size_t len = packet->encrypted.size;
    if (len > sizeof(decrypted_buf)) len = sizeof(decrypted_buf);

    // AES-CTR - потоковый шифр, поэтому расшифровка делается той же функцией, что и шифрование
    aes_128_ctr_encrypt(DEFAULT_CHANNEL_KEY, nonce, packet->encrypted.bytes, len, decrypted_buf);

    return deserialize_data(decrypted_buf, len);
}

const char* handle_data(meshtastic_Data data) {
    static char text_buf[MESHSTATIC_MAX_PAYLOAD_LEN + 1];

    if (data.portnum != MESHTASTIC_PORTNUM_TEXT_MESSAGE_APP) {
        return NULL;
    }

    size_t len = data.payload.size;
    if (len > MESHSTATIC_MAX_PAYLOAD_LEN) len = MESHSTATIC_MAX_PAYLOAD_LEN;

    memcpy(text_buf, data.payload.bytes, len);
    text_buf[len] = '\0';

    return text_buf;
}

void send_mesh_text_message(const char* text) {
    meshtastic_Data data = send_data(text);

    static uint8_t serialized_data[256];
    size_t data_len = serialize_data(&data, serialized_data);

    // формируем MeshPacket - бродкаст всем узлам, максимум 3 прыжка через репитеры
    meshtastic_MeshPacket packet = {0};
    packet.to = MESHTASTIC_BROADCAST_ADDR;
    packet.from = esp32_get_node_id();
    packet.id = esp32_random_u32();
    packet.hop_limit = MESHTASTIC_DEFAULT_HOP_LIMIT;
    packet.encrypted.bytes = serialized_data; // пока открытый текст - send_mesh_packet зашифрует его перед отправкой
    packet.encrypted.size = data_len;

    send_mesh_packet(&packet);
}
