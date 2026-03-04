#include <boost/asio/streambuf.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);

    auto endpoints = resolver.resolve("127.0.0.1", "8090");
    tcp::socket socket(io_context);

    boost::asio::connect(socket, endpoints);
    std::string message = "Hello from the client!\n";

    boost::asio::write(socket, boost::asio::buffer(message));

    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, '\n');

    std::cout << std::string{buffers_begin(response.data()), buffers_end(response.data())};

    return 0;
}