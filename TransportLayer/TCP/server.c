#include <stdio.h>

typedef unsigned char byte;

struct tcp_protocol {
    byte source_port[16];
    byte destination_port[16];

    byte sequence_number[32];
    byte acknowledgment_number[32];

    byte data_offset[4];
    byte reserved[3];

    byte flags[9];
    byte window_size[16];

    byte checksum[16];
    byte urgent_pointer[16];
};

int send_tcp_packet(struct tcp_protocol *packet) {
    // Code to send TCP packet
    return 0;
}

int main() {
    printf("Starting TCP server!\n");
    return 0;
}