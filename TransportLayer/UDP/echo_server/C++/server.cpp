#include <boost/asio/ip/udp.hpp>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

int main() {
    // Занимаем порт 8090 для прослушивания входящих сообщений
    boost::asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), 8090));

    std::cout << "UDP Echo Server is running on port 8090..." << std::endl;

    char data[1024];
    udp::endpoint sender_endpoint; // Создаём место куда кидаем информацию о клиенте, который нам что-то отправил
    boost::system::error_code ec; // Создаём объект для хранения ошибок, которые могут возникнуть при приёме или отправке данных

    // Получаем данные от клиента
    size_t length = socket.receive_from(boost::asio::buffer(data), sender_endpoint, 0, ec);
    if (ec) {
        std::cerr << "Receive error: " << ec.message() << std::endl;
        return 1;
    }

    // Выводим информацию о клиенте и полученное сообщение
    std::string received(data, length);
    std::cout << "Received from client: " << sender_endpoint.address().to_string() << ":" << sender_endpoint.port() << " " << received << std::endl;

    // Отправляем обратно клиенту полученное сообщение
    std::string message = "Hello from server!";
    socket.send_to(boost::asio::buffer(message), sender_endpoint, 0, ec);
    if (ec) {
        std::cerr << "Send error: " << ec.message() << std::endl;
        return 1;
    }

    return 0;
}