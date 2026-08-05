#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// я инвалидище, поэтому без протобаф
typedef struct _pb_bytes {
    const uint8_t *bytes;
    size_t size;
} pb_bytes_t;

typedef struct _meshtastic_Data {
    uint32_t portnum; // ID порта приложения (например, 1 для TEXT_MESSAGE)
    pb_bytes_t payload;  // Массив байт (текст сообщения)
    uint32_t id;
    bool want_response;
} meshtastic_Data;

typedef struct _meshtastic_MeshPacket {
    uint32_t to; // ID получателя (0xFFFFFFFF - бродкаст)
    uint32_t from; // ID отправителя
    pb_bytes_t encrypted; // Зашифрованные данные или сырой Data (если без шифрования)
    uint32_t id; // Случайный ID пакета
    uint32_t hop_limit; // Ограничение по прыжкам ретрансляции
} meshtastic_MeshPacket;

#define MESHTASTIC_PORTNUM_TEXT_MESSAGE_APP 1
#define MESHTASTIC_BROADCAST_ADDR 0xFFFFFFFFu
#define MESHTASTIC_DEFAULT_HOP_LIMIT 3

// Аппаратно-зависимые функции - реализуются под конкретную плату/радиочип
extern void aes_128_ctr_encrypt(const uint8_t *key, const uint8_t *nonce, const uint8_t *input, size_t len, uint8_t *output);
extern void sx1262_send_packet(const uint8_t *buffer, size_t len);
extern uint32_t esp32_get_node_id(void); // наш NodeID (обычно вычисляется из MAC-адреса ESP32)
extern uint32_t esp32_random_u32(void);  // аппаратный ГСЧ (например, esp_random())

// Собственно сама отправка сообщения в Meshtastic (текстовый чат)
void send_mesh_text_message(const char* text);

meshtastic_Data send_data(const char* text); // тут создаём Data и заполняем его поля
meshtastic_MeshPacket send_mesh_packet(meshtastic_MeshPacket *packet); // тут шифруем содержимое пакета и отправляем его по радиоканалу
meshtastic_Data receive_mesh_packet(meshtastic_MeshPacket *packet); // тут принимаем пакет, расшифровываем наш пакет
const char* handle_data(meshtastic_Data data); // тут обрабатываем полученные данные
