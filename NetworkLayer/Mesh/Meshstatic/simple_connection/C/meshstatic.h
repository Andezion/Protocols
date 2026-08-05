#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern void aes_128_ctr_encrypt(const uint8_t *key, uint8_t *nonce, uint8_t *input, size_t len, uint8_t *output);
extern void sx1262_send_packet(uint8_t *buffer, size_t len);

// Собственно сама отправка сообщения в Meshtastic (текстовый чат)
void send_mesh_text_message(const char* text);

void send_data(); // тут создаём Data и заполняем его поля 
void send_mesh_packet(); // тут создаем MeshPacket, шифруем и отправляем его по радиоканалу
void receive_mesh_packet(); // тут принимаем пакет, расшифровываем наш пакет
void handle_data(); // тут обрабатываем полученные данные