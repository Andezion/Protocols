#include <boost/asio/io_context.hpp>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);


    
    return 0;
}