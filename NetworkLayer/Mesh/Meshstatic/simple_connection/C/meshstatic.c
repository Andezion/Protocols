#include "meshstatic.h"

bool encode_bytes_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
    buffer_arg_t *buf = (buffer_arg_t*)*arg;
    if (!pb_encode_tag_for_field(stream, field)) return false;
    return pb_encode_string(stream, buf->data, buf->length);
}

void send_mesh_text_message(const char* text) {
    // создаем и кодируем внутреннюю структуру Data (Полезная нагрузка)
    meshtastic_Data app_data = meshtastic_Data_init_default;
    app_data.portnum = 1; // TEXT_MESSAGE_APP (ID для обычного чата)
    
    buffer_arg_t text_arg = { .data = (uint8_t*)text, .length = strlen(text) };
    app_data.payload.funcs.encode = encode_bytes_callback;
    app_data.payload.arg = &text_arg;

    uint8_t serialized_data[256];
    pb_ostream_t data_stream = pb_ostream_from_buffer(serialized_data, sizeof(serialized_data));
    
    if (!pb_encode(&data_stream, meshtastic_Data_fields, &app_data)) {
        return; // ошибка кодирования текста
    }
    size_t data_len = data_stream.bytes_written;

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

    // создаем финальный MeshPacket для радиоэфира
    meshtastic_MeshPacket packet = meshtastic_MeshPacket_init_default;
    packet.to = 0xFFFFFFFF; // бродкаст (всем узлам)
    packet.from = from_node;
    packet.id = packet_id;
    packet.hop_limit = 3; // пакет переродится максимум 3 раза через репитеры

    // передаем зашифрованный буфер в поле encrypted
    buffer_arg_t crypto_arg = { .data = encrypted_data, .length = data_len };
    packet.encrypted.funcs.encode = encode_bytes_callback;
    packet.encrypted.arg = &crypto_arg;

    uint8_t final_radio_buffer[384];
    pb_ostream_t packet_stream = pb_ostream_from_buffer(final_radio_buffer, sizeof(final_radio_buffer));

    if (!pb_encode(&packet_stream, meshtastic_MeshPacket_fields, &packet)) {
        return; // ошибка финального кодирования
    }
    size_t final_len = packet_stream.bytes_written;

    // отправка в радиочип LoRa
    // теперь final_radio_buffer содержит "чистый" байт-код протокола Meshtastic
    // передаем его в драйвер трансивера (например, SX1262) по SPI
    sx1262_send_packet(final_radio_buffer, final_len);
}