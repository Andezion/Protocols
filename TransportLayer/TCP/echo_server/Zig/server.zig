const http = @import("http");
const std = @import("std");

pub fn main() !void {
    const allocator = std.heap.page_allocator;
    const server = try http.Server.init(allocator, 8080);
    defer server.deinit();

    while (true) {
        const conn = try server.accept();
        defer conn.close();

        const request = try conn.readRequest();
        defer request.deinit();

        const response = http.Response{
            .statusCode = 200,
            .headers = &[_]http.Header{
                .{ .name = "Content-Type", .value = "text/plain" },
            },
            .body = "Hello, World!",
        };

        try conn.writeResponse(response);
    }
}
