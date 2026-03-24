const std = @import("std");

pub fn main() !void {
    const allocator = std.heap.page_allocator;
    const stdout = std.io.getStdOut().writer();

    const address = try std.net.Address.parseIp4("127.0.0.1:12345");
    const socket = try std.net.StreamSocket(.{});
    defer socket.close();
    try socket.connect(address);
}
