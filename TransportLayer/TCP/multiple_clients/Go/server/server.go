package main

import (
	"fmt"
	"log"
	"net"
)

// Этот сервер может обрабатывать несколько клиентов одновременно, используя горутины
func handleConnection(conn net.Conn) {
	defer conn.Close() // точно закрываем соединение после обработки клиента

	// Читаем данные от клиента
	buf := make([]byte, 1024)
	n, err := conn.Read(buf)
	if err != nil {
		log.Println("Error reading from client:", err)
		return
	}

	// Выводим полученное сообщение и адрес клиента, затем отправляем его обратно
	fmt.Printf("Received from client %s: %s\n", conn.RemoteAddr().String(), string(buf[:n]))
	_, err = conn.Write(buf[:n])
	if err != nil {
		log.Println("Error writing to client:", err)
		return
	}

	fmt.Printf("Echoed back to client %s.\n", conn.RemoteAddr().String())
}

func main() {
	// как и в обычном, создаем TCP-листенер на порту 8090
	listener, err := net.Listen("tcp", ":8090")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Echo server is listening on port 8090...")

	// вот тут уже разница, тут бесконечный цикл, который принимает новых клиентов и запускает для каждого горутину handleConnection
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Fatal(err)
		}

		fmt.Printf("Client connected: %s\n", conn.RemoteAddr().String())

		go handleConnection(conn)
	}
}
