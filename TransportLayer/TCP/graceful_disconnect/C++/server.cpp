#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8090));

    std::cout << "Server is running on port 8090..." << std::endl;

    while (true) {
        try {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');

            std::string client_msg{buffers_begin(buffer.data()), buffers_end(buffer.data())};
            std::cout << "Received from client: " << client_msg;

            std::string response = "Hello from the server!\n";
            std::size_t written = boost::asio::write(socket, boost::asio::buffer(response));
            
            std::cout << "Sent " << written << " bytes to client." << std::endl;

            // Graceful shutdown - сигнализируем клиенту что данных больше не будет
            socket.shutdown(tcp::socket::shutdown_send);

            // Дочитываем всё что клиент мог прислать после нашего shutdown (до EOF)
            boost::system::error_code ec;
            std::ostringstream oss;

            char buf[1024];

            for (;;) {
                std::size_t len = socket.read_some(boost::asio::buffer(buf), ec);

                if (len > 0) {
                    oss.write(buf, static_cast<std::streamsize>(len));
                }
                if (ec == boost::asio::error::eof) {
                    break;
                }
                if (ec) {
                    throw boost::system::system_error(ec);
                }
            }
            if (!oss.str().empty()) {
                std::cout << "Remaining data from client: " << oss.str() << std::endl;
            }

            socket.close();
            std::cout << "Connection closed gracefully." << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "Connection error: " << e.what() << std::endl;
            // Не выходим - продолжаем принимать следующие соединения
        }
    }

    return 0;
}
