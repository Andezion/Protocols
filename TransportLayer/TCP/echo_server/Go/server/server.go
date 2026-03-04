package main

import (
	"fmt"
	"log"
	"net"
)

func main() {
	// Биндим и слушаем TCP-порт 8090 на всех интерфейсах
	// Если порт уже занят, программа завершится с ошибкой
	listener, err := net.Listen("tcp", ":8090")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Echo server is listening on port 8090...")

	// Принимаем одно входящее соединение от клиента
	conn, err := listener.Accept()
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Client connected: %s\n", conn.RemoteAddr().String())

	// Читаем данные от клиента (ожидаем одну строку с '\n')
	buf := make([]byte, 1024)
	n, err := conn.Read(buf)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Received from client: %s\n", string(buf[:n]))

	// Эхо-ответ: отправляем обратно то же самое, что получили от клиента
	_, err = conn.Write(buf[:n])
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Echoed back to client.")

	conn.Close()
}
