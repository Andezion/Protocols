package main

import (
	"fmt"
	"log"
	"net"
)

func main() {
	// Говорим, что создали UDP сервер, который слушает порт 8090
	pc, err := net.ListenPacket("udp", ":8090")
	if err != nil {
		log.Fatal(err)
	}

	// Закрываем соединение при завершении функции main
	defer pc.Close()

	fmt.Println("Echo server is listening on port 8090...")

	buf := make([]byte, 2048) // Буфер для чтения данных от клиента

	// Принимаем данные от клиента и отправляем их обратно
	n, addr, err := pc.ReadFrom(buf)
	if err != nil {
		log.Println("read error:", err)
	}

	fmt.Printf("Received %d bytes from %s: %s\n", n, addr.String(), string(buf[:n]))

	// Отправляем полученные данные обратно клиенту
	_, err = pc.WriteTo(buf[:n], addr)
	if err != nil {
		log.Println("write error:", err)
	}
}
