package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"strings"
)

func main() {
	// Слушаем порт 12345 на всех интерфейсах
	ln, err := net.Listen("tcp", ":12345")
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("Echo server is listening on port 12345...")

	conn, err := ln.Accept()
	if err != nil {
		log.Println("Error accepting connection:", err)
		return
	}
	fmt.Printf("Client connected: %s\n", conn.RemoteAddr().String())

	// Читаем данные от клиента
	reader := bufio.NewReader(conn)
	for {
		msg, err := reader.ReadString('\n')
		if err != nil {
			log.Println("Error reading from client:", err)
			break
		}
		msg = strings.TrimSpace(msg)
		fmt.Printf("Received from client: %s\n", msg)

		// Отправляем обратно клиенту
		response := fmt.Sprintf("Echo: %s\n", msg)
		_, err = conn.Write([]byte(response))
		if err != nil {
			log.Println("Error writing to client:", err)
			break
		}
		fmt.Println("Echoed back to client.")
	}

	conn.Close()
}
