#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tcp.h"

int main(void) {
	const char *server_ip = "127.0.0.1";
	uint16_t server_port = 12345;

	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) { 
        perror("socket"); 
        return 1; 
    }

	struct sockaddr_in serv = { 0 };

	serv.sin_family = AF_INET;
	serv.sin_port = htons(server_port);

	inet_pton(AF_INET, server_ip, &serv.sin_addr);

	uint32_t isn;
	if (tcp_connect(s, (struct sockaddr *) &serv, sizeof(serv), &isn) == 0) {
		printf("Connected to %s:%u (ISN = %u)\n", server_ip, server_port, isn);
	} else {
		printf("Connection failed or timed out.\n");
	}

	close(s);
	return 0;
}
