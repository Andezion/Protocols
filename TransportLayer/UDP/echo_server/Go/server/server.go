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

}
