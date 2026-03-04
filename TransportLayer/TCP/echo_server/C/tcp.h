#ifndef SIMPLE_TCP_H
#define SIMPLE_TCP_H

#include <stdint.h>    // для uint16_t
#include <stddef.h>    // для size_t
#include <sys/types.h> // для ssize_t
#include <sys/socket.h> // для struct sockaddr и socklen_t

/*
 * TCP-флаги (приведены для справки - при использовании SOCK_STREAM
 * ядро управляет ими автоматически, нам вручную их выставлять не нужно):
 *
 *  FIN - завершение соединения (инициирует четырёхстороннее закрытие)
 *  SYN - синхронизация ISN при трёхстороннем рукопожатии
 *  RST - немедленный сброс соединения при ошибке
 *  PSH - немедленная передача данных без ожидания заполнения буфера
 *  ACK - подтверждение получения данных
 *  URG - пакет содержит срочные (out-of-band) данные
 */
#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20

// Создаёт TCP-сокет (SOCK_STREAM) без привязки к порту
// Возвращает дескриптор сокета при успехе, -1 при ошибке
int tcp_socket(void);

// Создаёт TCP-сокет и привязывает его к указанному порту на всех интерфейсах
// Возвращает дескриптор сокета при успехе, -1 при ошибке
int tcp_socket_bind(uint16_t port);

// Переводит сокет в пассивный режим ожидания входящих соединений
// backlog — максимальная длина очереди ожидающих соединений
// Возвращает 0 при успехе, -1 при ошибке.
int tcp_listen(int sockfd, int backlog);

// Принимает входящее TCP-соединение (блокирующий вызов)
// Трёхстороннее рукопожатие (SYN -> SYN-ACK -> ACK) выполняет ядро
// Заполняет clientaddr и *addrlen адресом подключившегося клиента
// Возвращает дескриптор нового соединения при успехе, -1 при ошибке
int tcp_accept(int sockfd, struct sockaddr *clientaddr, socklen_t *addrlen);

// Устанавливает TCP-соединение с сервером
// Трёхстороннее рукопожатие (SYN -> SYN-ACK -> ACK) выполняет ядро
// Возвращает 0 при успехе, -1 при ошибке
int tcp_connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);

// Отправляет ровно len байт данных по установленному TCP-соединению
// Повторяет send() при частичной отправке (гарантирует полную передачу)
// Возвращает количество отправленных байт при успехе, -1 при ошибке
ssize_t tcp_send(int sockfd, const void *data, size_t len);

// Принимает до buflen байт данных из TCP-соединения
// Возвращает количество принятых байт, 0 при закрытии соединения, -1 при ошибке
ssize_t tcp_recv(int sockfd, void *buf, size_t buflen);

#endif


