#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct _meshtastic_Data {
    uint32_t portnum; // ID порта приложения (например, 1 для TEXT_MESSAGE)
    pb_callback_t payload;  // Массив байт (текст сообщения)
    uint32_t id;
    bool want_response;
} meshtastic_Data;

typedef struct _meshtastic_MeshPacket {
    uint32_t to; // ID получателя (0xFFFFFFFF - бродкаст)
    uint32_t from; // ID отправителя
    pb_callback_t encrypted;// Зашифрованные данные или сырой Data (если без шифрования)
    uint32_t id; // Случайный ID пакета
    uint32_t hop_limit; // Ограничение по прыжкам ретрансляции
} meshtastic_MeshPacket;

extern void aes_128_ctr_encrypt(const uint8_t *key, uint8_t *nonce, uint8_t *input, size_t len, uint8_t *output);
extern void sx1262_send_packet(uint8_t *buffer, size_t len);

// Собственно сама отправка сообщения в Meshtastic (текстовый чат)
void send_mesh_text_message(const char* text);

meshtastic_Data send_data(const char* text); // тут создаём Data и заполняем его поля 
meshtastic_MeshPacket send_mesh_packet(meshtastic_MeshPacket *packet); // тут создаем MeshPacket, шифруем и отправляем его по радиоканалу
meshtastic_Data receive_mesh_packet(meshtastic_MeshPacket *packet); // тут принимаем пакет, расшифровываем наш пакет
const char* handle_data(meshtastic_Data data); // тут обрабатываем полученные данные