const std = @import("std");
const posix = std.posix;

pub fn main() !void {
    // Создваем UDP сокет. AF.INET - IPv4, SOCK.DGRAM - UDP, 0 - протокол по умолчанию
    const sock = try posix.socket(posix.AF.INET, posix.SOCK.DGRAM, 0);
    // Закрываем сокет при выходе из функции
    defer posix.close(sock);

    // Привязываем сокет к адресу и порту
    const address = try std.net.Address.resolveIp("127.0.0.1", 12345);

    // Связываем сокет с адресом и портом, чтобы сервер мог принимать данные
    try posix.bind(sock, &address.any, address.getOsSockLen());

    std.debug.print("UDP echo server listening on port 12345...\n", .{});

    // Бесконечный цикл для обработки входящих сообщений от клиентов
    while (true) {
        // Буфер для приема данных от клиента
        var buf: [1024]u8 = undefined;
        // Структура для хранения адреса клиента, от которого пришло сообщение
        var client_addr: posix.sockaddr = undefined;
        // Длина структуры адреса клиента, которая будет обновляться функцией recvfrom
        var client_addr_len: posix.socklen_t = @sizeOf(posix.sockaddr);

        // Принимаем данные от клиента. recvfrom заполняет client_addr и client_addr_len
        const n = try posix.recvfrom(sock, &buf, 0, &client_addr, &client_addr_len);
        if (n == 0) {
            continue;
        }

        const msg = buf[0..n];
        std.debug.print("Client says: {s}", .{msg});

        // Отправляем обратно полученное сообщение клиенту, используя адрес и порт, от которых пришло сообщение
        _ = try posix.sendto(sock, msg, 0, &client_addr, client_addr_len);
    }
}
