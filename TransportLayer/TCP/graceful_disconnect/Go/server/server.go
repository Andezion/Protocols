package main

import (
	"fmt"
	"net"
)

func main() {
	listener, err := net.Listen("tcp", ":8090")
	if err != nil {
		panic(err)
		// log.Fatal(err)
	}

	fmt.Println("Server is listening on port 8090...")

	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("Error accepting connection:", err)
			continue
		}

		fmt.Printf("Client connected: %s\n", conn.RemoteAddr().String())

		go func(c net.Conn) {
			defer c.Close() // гарантируем, что соединение будет закрыто после обработки клиента

			buf := make([]byte, 1024)
			n, err := c.Read(buf)
			if err != nil {
				fmt.Println("Error reading from client:", err)
				return
			}

			fmt.Printf("Received from client %s: %s\n", c.RemoteAddr().String(), string(buf[:n]))
			_, err = c.Write(buf[:n])
			if err != nil {
				fmt.Println("Error writing to client:", err)
				return
			}

			fmt.Printf("Echoed back to client %s.\n", c.RemoteAddr().String())
		}(conn)
	}
}
