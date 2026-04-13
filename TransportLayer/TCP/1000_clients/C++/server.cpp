#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <algorithm>
#include <atomic>

using boost::asio::ip::tcp;

std::mutex clients_mutex;
std::vector<std::shared_ptr<tcp::socket>> clients;
std::atomic<bool> running{true};

void broadcast(const std::string& msg, std::shared_ptr<tcp::socket> sender) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto& client : clients) {
        if (client.get() == sender.get() || !client->is_open()) continue;
        boost::system::error_code ec;
        [[maybe_unused]] std::size_t written =
            boost::asio::write(*client, boost::asio::buffer(msg), ec);
    }
}

void remove_client(std::shared_ptr<tcp::socket> socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(
        std::remove_if(clients.begin(), clients.end(),
            [&](const auto& s) { return s.get() == socket.get(); }),
        clients.end()
    );
    std::cout << "Client disconnected. Total clients: " << clients.size() << "\n";
}

void handle_client(std::shared_ptr<tcp::socket> socket) {
    while (running) {
        boost::asio::streambuf buf;
        boost::system::error_code ec;
        [[maybe_unused]] std::size_t bytes =
            boost::asio::read_until(*socket, buf, '\n', ec);

        if (ec) break; 

        std::string msg{buffers_begin(buf.data()), buffers_end(buf.data())};
        std::cout << "[broadcast] " << msg << std::flush;
        broadcast(msg, socket);
    }

    try { socket->close(); } catch (...) {}
    remove_client(socket);
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8090));

        std::cout << "Server running on port 8090.\n"
                  << "Press 'q' + Enter to stop.\n";

        std::thread accept_thread([&]() {
            while (running) {
                auto socket = std::make_shared<tcp::socket>(io_context);
                boost::system::error_code ec;
                acceptor.accept(*socket, ec);

                if (ec) break; 

                {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    clients.push_back(socket);
                    std::cout << "New client connected. Total clients: "
                              << clients.size() << "\n";
                }

                std::thread(handle_client, socket).detach();
            }
        });

        std::string input;
        while (std::getline(std::cin, input)) {
            if (input == "q" || input == "Q") {

                running = false;

                try { 
                    acceptor.close(); 
                } 
                catch (...) {

                }
                
                std::lock_guard<std::mutex> lock(clients_mutex);
                for (auto& client : clients) {
                    try { 
                        client->close(); 
                    } 
                    catch (...) {

                    }
                }
                break;
            }
        }

        accept_thread.join();
        std::cout << "Server stopped.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
