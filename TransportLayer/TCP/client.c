#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tcp.h"

int main(void) {
	const char *server_ip = "127.0.0.1"; // тут просто выбрали IP адрес сервера, можно любой другой, но он должен быть доступен и слушать на нужном порту
	uint16_t server_port = 12345; // тут просто выбрали порт сервера, можно любой от 1024 до 65535, но он должен быть свободным и сервер должен слушать на нём

	int s = socket(AF_INET, SOCK_DGRAM, 0); // создаём UDP сокет, который мы будем использовать для имитации TCP
	if (s < 0) { 
        perror("socket"); 
        return 1; 
    }

	struct sockaddr_in serv = { 0 }; // просто создание структуры для адреса сервера, мы будем использовать её для указания, куда мы хотим подключиться

	serv.sin_family = AF_INET; // мы используем IPv4
	serv.sin_port = htons(server_port);

	inet_pton(AF_INET, server_ip, &serv.sin_addr); // преобразуем IP адрес сервера из текстового вида в бинарный и сохраняем его в структуре serv

	uint32_t isn; // храним ISN, который мы получаем от сервера в процессе рукопожатия, если нам нужно его вернуть, то мы передаем указатель на эту переменную в функцию tcp_connect, иначе мы передаем NULL
	if (tcp_connect(s, (struct sockaddr *) &serv, sizeof(serv), &isn) == 0) {
		// пробуем сделать рукопожатие с сервером, если оно прошло успешно, то мы получаем ISN сервера в переменную isn, иначе мы возвращаем -1
		printf("Connected to %s:%u (ISN = %u)\n", server_ip, server_port, isn);
	} else {
		printf("Connection failed or timed out.\n");
	}

	close(s);
	return 0;
}
