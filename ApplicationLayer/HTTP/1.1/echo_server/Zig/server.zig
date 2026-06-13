const std = @import("std");
const log = std.log;
const os = std.os;
const http = std.http;
const Io = std.io;

const Request = std.http.Server.Request;
const Response = std.http.Server.Response;

pub fn main(init: std.process.Init) !void {
    const io = init.io;

    const addr = try std.net.IpAddress.parse("127.0.0.1", 8080);
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

fn accept(stream: std.net.Stream, io: Io) !void {
    defer stream.close(io);

    log.info("New client: {s}", .{stream.peerAddress(io)});

    var recv_buffer: [1024]u8 = undefined;
    var send_buffer: [1024]u8 = undefined;

    var stream_reader = stream.reader(io, &recv_buffer);
    var stream_writer = stream.writer(io, &send_buffer);

    var server = std.http.Server.init(&stream_reader.interface, &stream_writer.interface);

    while (server.reader.state == .ready) {
        var request = server.receiveHead() catch |err| switch (err) {
            error.HttpConnectionClosed => return,
            else => {
                log.err("Failed to receive request: {s}", .{@errorName(err)});
                return err;
            },
        };

        switch (request.upgradeRequested()) {
            .none => {
                try serverHTTP(&request);
            },
            .other => |other_protocol| {
                log.err("Unsupported upgrade protocol: {s}", .{other_protocol});
            },
        }
    }
}

fn serverHTTP(request: *Request) !void {
    try request.respond(
        "hi bitch",
        .{
            .extra_headers = .{
                .{ .name = "custon-header", .value = "custon value" },
            },
        },
    );
}
