#include <boost/asio/streambuf.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class Client {
private:
    int id{};
public:
    Client(int id) : id(id) {
        // Создаем io_context и resolver для разрешения адреса сервера
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);

        // Разрешаем адрес сервера и порт
        auto endpoints = resolver.resolve("127.0.0.1", "8090");
        tcp::socket socket(io_context);

        boost::asio::connect(socket, endpoints);
        std::string message = "Hello from the client " + std::to_string(id) + "\n";

        boost::asio::write(socket, boost::asio::buffer(message));

        boost::system::error_code ec;
        while (true) {
            // Читаем ответ от сервера до символа новой строки
            boost::asio::streambuf response;
            boost::asio::read_until(socket, response, '\n');

            // Преобразуем полученные данные в строку и выводим на консоль
            std::string client_msg{buffers_begin(response.data()), buffers_end(response.data())};
            std::cout << "Received from server: " << client_msg << std::endl;

            if (ec == boost::asio::error::eof) {
                break;
            }
            if (ec) {
                throw boost::system::system_error(ec);
            }
        }
    }

};

int main() {
    int i = 1;
    while (i < 1001) {
        Client client(i);
        i++;
    }

    return 0;
}