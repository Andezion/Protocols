package main

import (
	"fmt"
	"log"
	"net"
)

func main() {
	listener, err := net.Listen("tcp", ":8090")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Echo server is listening on port 8090...")

	conn, err := listener.Accept()
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Client connected: %s\n", conn.RemoteAddr().String())

	buf := make([]byte, 1024)
	n, err := conn.Read(buf)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Received from client: %s\n", string(buf[:n]))

	_, err = conn.Write(buf[:n])
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Echoed back to client.")

	conn.Close()
}
