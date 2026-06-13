const std = @import("std");
const log = std.log;
const net = std.net;
const os = std.os;
const http = std.http;

const Request = std.http.Server.Request;
const Response = std.http.Server.Response;

pub fn main(init: std.process.Init) !void {
    const io = init.io;

    const addr = try net.IpAddress.parse("127.0.0.1", 8080);
    var server = try addr.listen(io, .{ .reuse_address = true });
    defer server.close(io);

    log.info("Started shitty echo server on: {f}", .{addr});

    while (true) {
        const stream = server.accept(io) catch |err| {
            log.err("Failed to accept conn: {s}", .{@errorName(err)});
            continue;
        };
    }
}
