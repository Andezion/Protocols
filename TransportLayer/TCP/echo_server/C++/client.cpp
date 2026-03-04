#include <iostream>
#include <array>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::err << "Usage: client <host>" << std::endl;
            return 1;
        }
    }

    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);

    return 0;
}