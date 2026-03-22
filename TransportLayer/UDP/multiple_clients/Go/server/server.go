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

	fmt.Println("UDP server is listening on port 8090...")

	for {
		buf := make([]byte, 2048)

		n, addr, err := listener.ReadFrom(buf)
		if err != nil {
			log.Println("read error:", err)
			continue
		}

		fmt.Printf("Received %d bytes from %s: %s\n", n, addr.String(), string(buf[:n]))

		_, err = listener.WriteTo(buf[:n], addr)
		if err != nil {
			log.Println("write error:", err)
		}

	}
}
