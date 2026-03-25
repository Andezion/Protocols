#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);

        auto endpoints = resolver.resolve("127.0.0.1", "8090");
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        std::string message = "Hello from the client!\n";
        std::size_t written = boost::asio::write(socket, boost::asio::buffer(message));
        std::cout << "Sent " << written << " bytes to server." << std::endl;

        // Graceful shutdown: сигнализируем серверу что данных больше не будет
        socket.shutdown(tcp::socket::shutdown_send);

        // Дочитываем ответ сервера до EOF
        boost::system::error_code ec;
        std::ostringstream oss;

        char buf[1024];

        // Читаем данные от сервера до тех пор, пока не получим EOF (сервер закрыл соединение)
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
        std::cout << "Received from server: " << oss.str();

        socket.close();
        std::cout << "Connection closed gracefully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
