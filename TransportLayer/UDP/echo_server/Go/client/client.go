package main

import (
	"fmt"
	"log"
	"net"
	"os"
)

func main() {
	addr := "127.0.0.1:8090"
	conn, err := net.Dial("udp", addr)
	if err != nil {
		log.Fatalf("Failed to connect to %s: %v", addr, err)
	}

	defer conn.Close()

	fmt.Printf("Connected to server %s\n", addr)

	message := "Hello from client\n"
	_, err = conn.Write([]byte(message))
	if err != nil {
		log.Fatalf("Failed to send message: %v", err)
	}

	buf := make([]byte, 1024)
	n, err := conn.Read(buf)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error reading response:", err)
		return
	}

	fmt.Printf("Received from server: %s", string(buf[:n]))

	fmt.Println("Client finished one handshake and will exit.")
}
