const std = @import("std");

pub fn main() !void {
    // Подключаемся к серверу по адресу
    const address = try std.net.Address.resolveIp("127.0.0.1", 12345);
    // Создаем TCP-сервер, который будет слушать указанный адрес
    var server = try address.listen(.{ .reuse_address = true });
    // Инициализируем сервер, чтобы он был готов принимать соединения
    defer server.deinit();

    std.debug.print("TCP echo server listening on port 12345...\n", .{});

    while (true) {
        // Принимаем входящее соединение от клиента
        const conn = try server.accept();
        // Закрываем соединение при выходе из блока, чтобы освободить ресурсы
        defer conn.stream.close();

        var buf: [1024]u8 = undefined; // создаем буфер для чтения данных от клиента
        // Читаем данные от клиента
        const n = try conn.stream.read(&buf);
        if (n == 0) continue;

        const msg = buf[0..n];
        std.debug.print("Client says: {s}", .{msg});
        try conn.stream.writeAll("Hello from server!\n"); // отправляем ответ клиенту
    }
}
