#include <boost/asio/ip/udp.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

int main() {
    boost::asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), 8090)); 

    std::cout << "UDP Echo Server is running on port 8090..." << std::endl;

    std::string message = "Hello from the server!";

    char data[1024];
    udp::endpoint sender_endpoint;
    size_t length = socket.receive_from(boost::asio::buffer(data), sender_endpoint);

    std::cout << "Received from client: " << message << std::endl;

    socket.send_to(boost::asio::buffer(message), sender_endpoint);

    return 0;
}