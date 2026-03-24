#include <boost/asio/ip/udp.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

int main() {
    boost::asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), 8090)); 

    std::cout << "UDP Server is running on port 8090..." << std::endl;

    char data[1024];
    udp::endpoint sender_endpoint;
    boost::system::error_code ec;

    while (true) {
        size_t length = socket.receive_from(boost::asio::buffer(data), sender_endpoint, 0, ec);
        if (ec) {
            std::cerr << "Receive error: " << ec.message() << std::endl;
            continue; // Продолжать принимать следующие сообщения
        }

        std::string received(data, length);
        std::cout << "Received from client: " << sender_endpoint.address().to_string() << ":" << sender_endpoint.port() << " " << received << std::endl;

        std::string message = "Hello from server!";
        socket.send_to(boost::asio::buffer(message), sender_endpoint, 0, ec);
        if (ec) {
            std::cerr << "Send error: " << ec.message() << std::endl;
            continue; // Продолжать принимать следующие сообщения
        }
    }

    return 0;
}