const std = @import("std");
const posix = std.posix;

pub fn main() !void {
    const sock = try posix.socket(posix.AF.INET, posix.SOCK.DGRAM, 1);
    defer posix.close(sock);
}
