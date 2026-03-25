package main

import (
	"fmt"
	"log"
	"net"
)

func main() {
	// Создаем UDP сервер, который будет слушать на порту 8090
	listener, err := net.ListenPacket("udp", ":8090")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("UDP server is listening on port 8090...")

	// Бесконечный цикл для обработки входящих сообщений от клиентов
	for {
		// Буфер для хранения данных, полученных от клиента
		buf := make([]byte, 2048)

		// Читаем данные от клиента
		n, addr, err := listener.ReadFrom(buf)
		if err != nil {
			log.Println("read error:", err)
			continue
		}

		fmt.Printf("Received %d bytes from %s: %s\n", n, addr.String(), string(buf[:n]))

		// Отправляем ответ обратно клиенту
		_, err = listener.WriteTo(buf[:n], addr)
		if err != nil {
			log.Println("write error:", err)
		}
	}
}
