const std = @import("std");
const log = std.log;
const net = std.net;
const os = std.os;
const http = std.http;
const Io = std.io;

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
        const thread = std.Thread.spawn(.{}, accept, .{ stream, io }) catch |err| {
            log.err("Failed to spawn thread: {s}", .{@errorName(err)});
            continue;
        };

        thread.detach();
    }
}

fn accept(stream: net.Stream, io: Io) !void {
    defer stream.close(io);

    log.info("New client: {s}", .{stream.peerAddress(io)});

    var recv_buffer: [1024]u8 = undefined;
    var send_buffer: [1024]u8 = undefined;

    var stream_reader = stream.reader(io, &recv_buffer);
    var stream_writer = stream.writer(io, &send_buffer);
}
