const std = @import("std");
const posix = std.posix;

pub fn main() !void {
    // Создваем UDP сокет. AF.INET - IPv4, SOCK.DGRAM - UDP, 0 - протокол по умолчанию
    const sock = try posix.socket(posix.AF.INET, posix.SOCK.DGRAM, 0);
    defer posix.close(sock); // а тут закрываем сокет при выходе из функции

    // Разрешаем адрес сервера, к которому будем отправлять сообщения
    const address = try std.net.Address.resolveIp("127.0.0.1", 12345);

    const msg = "Hello from client!\n";
    // Отправляем сообщение серверу, используя адрес и порт сервера
    _ = try posix.sendto(sock, msg, 0, &address.any, address.getOsSockLen());

    var buf: [1024]u8 = undefined;
    // Принимаем ответ от сервера. recvfrom заполняет buf данными от сервера
    const n = try posix.recvfrom(sock, &buf, 0, null, null);
    std.debug.print("{s}", .{buf[0..n]});
}
