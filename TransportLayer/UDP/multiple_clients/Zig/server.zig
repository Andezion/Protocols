const std = @import("std");
const posix = std.posix;

pub fn main() !void {
    const sock = try posix.socket(posix.AF.INET, posix.SOCK_DGRAM, 1);
    defer posix.close(sock);

    const address = try std.net.Address.resolveIp("127.0.0.1", 8080);
    _ = try posix.bind(sock, &address.any, address.getOsSockLen());
    std.debug.print("UDP server listening on port 8080...\n", .{});
}
