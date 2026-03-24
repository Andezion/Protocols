const std = @import("std");

pub fn main() !void {
    const address = try std.net.Address.resolveIp("127.0.0.1", 12345);
    var server = try address.listen(.{ .reuse_address = true });
    defer server.deinit();

    std.debug.print("UDP echo server listening on port 12345...\n", .{});

    while (true) {
        var buf: [1024]u8 = undefined;
        const n = try server.recvFrom(&buf);
        if (n == 0) continue;

        const msg = buf[0..n];
        std.debug.print("Client says: {s}", .{msg});
        try server.sendTo(msg, address);
    }
}
