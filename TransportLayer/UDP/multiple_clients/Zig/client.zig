const std = @import("std");
const posix = std.posix;

pub fn main() !void {
    const sock = try posix.socket(posix.AF.INET, posix.SOCK.DGRAM, 0);
    defer posix.close(sock);

    const address = try std.net.Address.resolveIp("127.0.0.1", 8080);

    const msg = "Hello from client!\n";
    _ = try posix.sendto(sock, msg, 0, &address.any, address.getOsSockLen());
    var buf: [1024]u8 = undefined;
    const n = try posix.recvfrom(sock, &buf, 0, null, null);
    std.debug.print("{s}", .{buf[0..n]});
}
