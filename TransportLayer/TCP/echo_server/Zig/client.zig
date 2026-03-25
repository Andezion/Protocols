const std = @import("std");

pub fn main() !void {
    // Подключаемся к серверу по адресу
    const address = try std.net.Address.resolveIp("127.0.0.1", 12345);
    const stream = try std.net.tcpConnectToAddress(address); // тут мы создаем TCP-соединение с сервером

    // Закрываем соединение при выходе из функции
    defer stream.close();

    // Отправляем сообщение серверу
    try stream.writeAll("Hello from client!\n");

    var buf: [1024]u8 = undefined; // тут мы создаем буфер для чтения данных от сервера
    const n = try stream.read(&buf); // тут мы читаем данные от сервера в буфер и сохраняем количество прочитанных байт в переменной n
    if (n == 0) {
        std.debug.print("Server closed the connection.\n", .{});
        return;
    }
    std.debug.print("{s}", .{buf[0..n]});
}
