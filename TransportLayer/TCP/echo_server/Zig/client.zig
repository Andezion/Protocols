const std = @import("std");
const http = @import("http");

pub fn main() !void {
    const allocator = std.heap.page_allocator;
    const client = try http.Client.init(allocator);
    defer client.deinit();

    const request = http.Request{
        .method = "GET",
        .path = "/",
        .headers = &[_]http.Header{
            .{ .name = "Host", .value = "localhost:8080" },
        },
        .body = "",
    };

    const response = try client.sendRequest("localhost", 8080, request);
    defer response.deinit();

    std.debug.print("Response status: {}\n", .{response.statusCode});
    std.debug.print("Response body: {}\n", .{response.body});
}
