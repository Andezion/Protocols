#include <boost/asio/streambuf.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>

using boost::asio::ip::tcp;

int main() {
    try {
        // как и раньше, создаём io_context и resolver для разрешения адреса сервера
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);

        // Разрешаем адрес сервера и порт
        auto endpoints = resolver.resolve("127.0.0.1", "8090");
        tcp::socket socket(io_context);

        // Подключаемся к серверу
        boost::asio::connect(socket, endpoints);
        std::string message = "Hello from the client!\n";

        // Отправляем сообщение серверу
        boost::asio::write(socket, boost::asio::buffer(message));

        // Грейсфул-шатдаун: уведомляем сервер, что больше не будем слать данные
        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_send, ec);
        if (ec)
            std::cerr << "Shutdown send error: " << ec.message() << std::endl;

        // Дочитываем ответ от сервера до его закрытия (EOF)
        std::ostringstream oss;
        char buf[1024];
        for (;;) {
            std::size_t len = socket.read_some(boost::asio::buffer(buf), ec);
            if (len > 0) oss.write(buf, len);
            if (ec == boost::asio::error::eof) break;
            else if (ec) throw boost::system::system_error(ec);
        }
        std::string server_msg = oss.str();
        std::cout << "Received from server: " << server_msg << std::endl;

        socket.close(); // закрываем соединение
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 