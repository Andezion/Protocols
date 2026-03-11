package main

import (
	"fmt"
	"log"
	"net"
)

func main() {
	listener, err := net.ListenPacket("udp", ":8090")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Echo server is listening on port 8090...")

	conn, err := listener.Accept()
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Client connected: %s\n", conn.String())

	buf := make([]byte, 1024)
	n, addr, err := listener.ReadFrom(buf)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Received from client: %s\n", string(buf[:n]))

	_, err = listener.WriteTo(buf[:n], addr)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Echoed back to client.")

	conn.Close()
}
