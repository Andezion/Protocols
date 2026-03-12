#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

int main() {
	boost::asio::io_context io_context;

    udp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(udp::v4(), "127.0.0.1", "8090");

    udp::socket socket(io_context);
    socket.open(udp::v4());

    std::string msg = "Hello from client";
    socket.send_to(boost::asio::buffer(msg), *endpoints.begin());

    char reply[1024];
    udp::endpoint sender;
    size_t len = socket.receive_from(boost::asio::buffer(reply), sender);
    std::string received(reply, len);
    std::cout << "Server reply: " << received << std::endl;

	return 0;
}