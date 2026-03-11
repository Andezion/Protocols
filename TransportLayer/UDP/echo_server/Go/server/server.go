package main

import (
	"fmt"
	"log"
	"net"
)

func main() {
	pc, err := net.ListenPacket("udp", ":8090")
	if err != nil {
		log.Fatal(err)
	}
	defer pc.Close()

	fmt.Println("Echo server is listening on port 8090...")

	buf := make([]byte, 2048)

	n, addr, err := pc.ReadFrom(buf)
	if err != nil {
		log.Println("read error:", err)
	}

	fmt.Printf("Received %d bytes from %s: %s\n", n, addr.String(), string(buf[:n]))

	_, err = pc.WriteTo(buf[:n], addr)
	if err != nil {
		log.Println("write error:", err)
	}
}
