package main

import (
	"fmt"
	"log"
	"net"
)

func handleClient(conn net.Conn) {
	defer conn.Close()

	buf := make([]byte, 1024)
	n, err := conn.Read(buf)
	if err != nil {
		log.Printf("Failed to read from client %s: %v", conn.RemoteAddr().String(), err)
		return
	}

	fmt.Printf("Received from client %s: %s\n", conn.RemoteAddr().String(), string(buf[:n]))
	_, err = conn.Write(buf[:n])
	if err != nil {
		log.Printf("Failed to write to client %s: %v", conn.RemoteAddr().String(), err)
		return
	}

	fmt.Printf("Echoed back to client %s.\n", conn.RemoteAddr().String())
}

func main() {
	listener, err := net.Listen("tcp", ":8090")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Started echo server on port 8090...")

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("Failed to accept connection: %v", err)
			continue
		}

		fmt.Printf("Client connected: %s\n", conn.RemoteAddr().String())
		go handleClient(conn)
	}
}
