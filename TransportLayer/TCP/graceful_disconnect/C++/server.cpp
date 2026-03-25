#include <boost/asio/io_context.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp; 

int main() {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(),8090));

    std::cout << "Server is running on port 8090..." << std::endl;

    tcp::socket socket(io_context);
    acceptor.accept(socket);

    std::string message = "Hello from the server!";

    boost::asio::streambuf buffer;
    boost::asio::read_until(socket, buffer, '\n');

    std::string client_msg{buffers_begin(buffer.data()), buffers_end(buffer.data())};
    std::cout << "Received from client: " << client_msg << std::endl;

    boost::asio::write(socket, boost::asio::buffer(message + "\n"));

    socket.close();

    return 0;
}