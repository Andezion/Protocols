package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
)

func main() {
	// Подключаемся к серверу, который в проекте слушает порт 8090
	addr := "127.0.0.1:8090"
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		log.Fatalf("Failed to connect to %s: %v", addr, err)
	}
	defer conn.Close()

	fmt.Printf("Connected to server %s\n", addr)

	// Отправляем одну строку и завершаем
	message := "Hello from client\n"
	_, err = conn.Write([]byte(message))
	if err != nil {
		log.Fatalf("Failed to send message: %v", err)
	}

	// Читаем ответ (ожидаем одну строку с '\n')
	reader := bufio.NewReader(conn)
	resp, err := reader.ReadString('\n')
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error reading response:", err)
		return
	}
	fmt.Printf("Received from server: %s", resp)

	fmt.Println("Client finished one handshake and will exit.")
}
