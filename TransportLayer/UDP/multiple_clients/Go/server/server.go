package main

import (
	"log"
	"net"
)

func main() {
	listener, err := net.ListenPacket("udp", ":8090")
	if err != nil {
		log.Fatal(err)
	}

	for {
		buf := make([]byte, 2048)
		n, addr, err := listener.ReadFrom(buf)
		if err != nil {
			log.Println("read error:", err)
			continue
		}

		log.Printf("Received %d bytes from %s: %s\n", n, addr.String(), string(buf[:n]))
		_, err = listener.WriteTo(buf[:n], addr)
		if err != nil {
			log.Println("write error:", err)
		}
	}
}
