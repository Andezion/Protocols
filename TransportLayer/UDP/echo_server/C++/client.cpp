#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

int main() {
    // Создаём контекст ввода-вывода для работы с сетью
	boost::asio::io_context io_context;

    udp::resolver resolver(io_context); // Создаём резолвер для получения информации о сервере
    auto endpoints = resolver.resolve(udp::v4(), "127.0.0.1", "8090");

    // Создаём сокет для отправки и получения данных
    udp::socket socket(io_context);
    socket.open(udp::v4());

    std::string msg = "Hello from client"; // Сообщение, которое мы хотим отправить серверу
    socket.send_to(boost::asio::buffer(msg), *endpoints.begin());

    char reply[1024]; // Буфер для хранения ответа от сервера
    udp::endpoint sender; // Создаём объект для хранения информации о сервере, который нам ответил

    // Получаем ответ от сервера
    size_t len = socket.receive_from(boost::asio::buffer(reply), sender);

    std::string received(reply, len);
    std::cout << "Server reply: " << received << std::endl;

	return 0;
}