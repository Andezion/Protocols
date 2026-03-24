#include <boost/asio/ip/udp.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

int main() {
    boost::asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), 8090)); 

    std::cout << "UDP Server is running on port 8090..." << std::endl;
}