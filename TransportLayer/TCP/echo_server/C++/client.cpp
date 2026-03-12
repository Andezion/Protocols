#include <boost/asio/streambuf.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    // Создаем io_context и resolver для разрешения адреса сервера
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

    // Читаем ответ от сервера до символа новой строки
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, '\n');

    // Преобразуем полученные данные в строку и выводим на консоль
    std::string client_msg{buffers_begin(response.data()), buffers_end(response.data())};
    std::cout << "Received from server: " << client_msg << std::endl;

    return 0;
}