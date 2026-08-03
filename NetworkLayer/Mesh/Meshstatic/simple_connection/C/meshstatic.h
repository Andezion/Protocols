#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pb_encode.h"
#include "pb.h"
#include "mesh.pb.h"   

extern void aes_128_ctr_encrypt(const uint8_t *key, uint8_t *nonce, uint8_t *input, size_t len, uint8_t *output);
extern void sx1262_send_packet(uint8_t *buffer, size_t len);

typedef struct {
    const uint8_t *data;
    size_t length;
} buffer_arg_t;


typedef struct _meshtastic_Data {
    uint32_t portnum;       
    pb_callback_t payload;  
    uint32_t id;
    bool want_response;
} meshtastic_Data;

typedef struct _meshtastic_MeshPacket {
    uint32_t to;            
    uint32_t from;          
    pb_callback_t encrypted;
    uint32_t id;          
    uint32_t hop_limit;     
} meshtastic_MeshPacket;

// Callback для записи массивов байт в поток Protobuf
bool encode_bytes_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

// Собственно сама отправка сообщения в Meshtastic (текстовый чат)
void send_mesh_text_message(const char* text) ;