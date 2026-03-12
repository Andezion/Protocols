package main

import (
	"fmt"
	"log"
	"net"
	"os"
)

func main() {
	// Адрес сервера, к которому будем подключаться
	addr := "127.0.0.1:8090"
	conn, err := net.Dial("udp", addr) // Устанавливаем UDP соединение с сервером
	if err != nil {
		log.Fatalf("Failed to connect to %s: %v", addr, err)
	}

	// Закрываем соединение при завершении функции main
	defer conn.Close()

	fmt.Printf("Connected to server %s\n", addr)

	message := "Hello from client\n"
	_, err = conn.Write([]byte(message)) // Отправляем сообщение серверу
	if err != nil {
		log.Fatalf("Failed to send message: %v", err)
	}

	buf := make([]byte, 1024)
	n, err := conn.Read(buf) // Читаем ответ от сервера
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error reading response:", err)
		return
	}

	fmt.Printf("Received from server: %s", string(buf[:n]))

	fmt.Println("Client finished one handshake and will exit.")
}
