const std = @import("std");

pub fn main() !void {
    // Подключаемся к серверу по адресу
    const address = try std.net.Address.resolveIp("127.0.0.1", 12345);
    const stream = try std.net.tcpConnectToAddress(address); // тут мы создаем TCP-соединение с сервером

    defer stream.close();

    try stream.writeAll("Hello from client!\n");

    var buf: [1024]u8 = undefined;
    const n = try stream.read(&buf);
    std.debug.print("{s}", .{buf[0..n]});
}
