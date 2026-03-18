package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
)

// как и обычный клиент
func main() {
	// подключаемся к серверу, который должен быть запущен на этом же адресе и порту
	addr := "127.0.0.1:8090"
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		log.Fatalf("Failed to connect to %s: %v", addr, err)
	}

	defer conn.Close() // гарантируем, что соединение будет закрыто после завершения работы клиента

	fmt.Printf("Connected to server %s\n", addr)

	// клиент кричит
	message := "Hello from client\n"
	_, err = conn.Write([]byte(message))
	if err != nil {
		log.Fatalf("Failed to send message: %v", err)
	}

	// слушаем что кричат в ответ
	reader := bufio.NewReader(conn)
	resp, err := reader.ReadString('\n')
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error reading response:", err)
		return
	}

	fmt.Printf("Received from server: %s", resp)

	fmt.Println("Client finished one handshake and will exit.")

}
