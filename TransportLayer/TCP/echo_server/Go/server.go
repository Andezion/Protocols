package main

import (
	"container/list"
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

	clients := list.New()

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Println("Failed to accept connection:", err)
			continue
		}
		fmt.Println("New client connected:", conn.RemoteAddr())
		clients.PushBack(conn)

		go handleClient(conn, clients)
	}

}
