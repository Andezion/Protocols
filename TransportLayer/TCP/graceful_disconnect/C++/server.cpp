#include <boost/asio/io_context.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp; 

int main() {
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

    // Закрываем соединение
    socket.close();

    return 0;
}