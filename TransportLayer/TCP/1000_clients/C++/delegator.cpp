#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>

using boost::asio::ip::tcp;

std::atomic<bool> running{true};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./delegator <client_name> [message]\n"
                  << "  <client_name>  - name shown to all other clients\n"
                  << "  [message]      - optional one-shot message (then goes interactive)\n"
                  << "Example: ./delegator client1 popa\n";
        return 1;
    }

    const std::string name = argv[1];

    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "8090");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        std::cout << "Connected as '" << name << "'. Type messages (q = quit):\n> " << std::flush;

        // Send initial message if provided as second arg
        if (argc >= 3) {
            std::string msg = name + ": " + argv[2] + "\n";
            boost::asio::write(socket, boost::asio::buffer(msg));
        }

        // Thread: receive broadcasts from server and print them
        std::thread recv_thread([&socket]() {
            while (running) {
                boost::asio::streambuf buf;
                boost::system::error_code ec;
                boost::asio::read_until(socket, buf, '\n', ec);

                if (ec || !running) break;

                std::string msg{buffers_begin(buf.data()), buffers_end(buf.data())};
                // Move cursor to start, print incoming message, restore prompt
                std::cout << "\r[<<] " << msg << "> " << std::flush;
            }
        });

        // Main thread: read stdin, send to server
        std::string input;
        while (std::getline(std::cin, input)) {
            if (input == "q" || input == "Q") break;
            if (input.empty()) {
                std::cout << "> " << std::flush;
                continue;
            }
            std::string msg = name + ": " + input + "\n";
            boost::system::error_code ec;
            boost::asio::write(socket, boost::asio::buffer(msg), ec);
            if (ec) {
                std::cerr << "Server disconnected.\n";
                break;
            }
            std::cout << "> " << std::flush;
        }

        running = false;
        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);
        socket.close(ec);
        recv_thread.join();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
