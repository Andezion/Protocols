const std = @import("std");

pub fn main() !void {
    const address = try std.net.Address.resolveIp("127.0.0.1", 12345);
    const stream = try std.net.tcpConnectToAddress(address);
    defer stream.close();

    try stream.writeAll("Hello from client!\n");

    var buf: [1024]u8 = undefined;
    const n = try stream.read(&buf);
    std.debug.print("{s}", .{buf[0..n]});
}
