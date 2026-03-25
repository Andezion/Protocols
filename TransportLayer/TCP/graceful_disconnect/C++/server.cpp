#include <boost/asio/io_context.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>

using boost::asio::ip::tcp; 

int main() {
    // Создаем io_context и acceptor для прослушивания входящих соединений на порту 8090
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(),8090));

    std::cout << "Server is running on port 8090..." << std::endl;

    while (true) {
        try {
            // Создаем io_context и acceptor для прослушивания входящих соединений на порту 8090
            boost::asio::io_context io_context;
            tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(),8090));

            std::cout << "Server is running on port 8090..." << std::endl;

            // Ожидаем входящее соединение и принимаем его
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::string message = "Hello from the server!";

            // Читаем данные от клиента до символа новой строки
            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');

            // Преобразуем полученные данные в строку и выводим на консоль
            std::string client_msg{buffers_begin(buffer.data()), buffers_end(buffer.data())};
            std::cout << "Received from client: " << client_msg << std::endl;

            // Отправляем ответ клиенту
            boost::asio::write(socket, boost::asio::buffer(message + "\n"));

            // Грейсфул-шатдаун: сообщаем пиру, что мы больше не будем отправлять данные
            boost::system::error_code ec;
            socket.shutdown(tcp::socket::shutdown_send, ec);
            if (ec)
                std::cerr << "Shutdown send error: " << ec.message() << std::endl;

            // Дочитываем все данные от клиента до закрытия соединения (EOF)
            std::ostringstream oss;
            char buf[1024];
            for (;;) {
                std::size_t len = socket.read_some(boost::asio::buffer(buf), ec);
                if (len > 0) oss.write(buf, len);
                if (ec == boost::asio::error::eof) break;
                else if (ec) throw boost::system::system_error(ec);
            }
            std::string remaining = oss.str();
            if (!remaining.empty())
                std::cout << "Remaining data from client after shutdown: " << remaining << std::endl;

            socket.close();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    return 0;
}