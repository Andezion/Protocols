#include <boost/asio/streambuf.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
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
    // место для ответа
    boost::asio::streambuf response;
    // Читаем ответ от сервера до символа новой строки
    boost::asio::read_until(socket, response, '\n');

    // трансформируем в текст ответ
    std::string client_msg{buffers_begin(response.data()), buffers_end(response.data())};
    std::cout << "Received from server: " << client_msg << std::endl;

    socket.close(); // закрываем соединение

    return 0;
} 