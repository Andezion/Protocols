package main

import (
	"bufio"
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

		go func(data []byte, clientAddr net.Addr) {
			scanner := bufio.NewScanner(bufio.NewReaderSize(nil, len(data)))
			scanner.Split(bufio.ScanLines)

			for scanner.Scan() {
				line := scanner.Text()
				fmt.Printf("Processing line from %s: %s\n", clientAddr.String(), line)
				_, err := listener.WriteTo([]byte(line), clientAddr)
				if err != nil {
					log.Println("write error:", err)
				}
			}
		}(buf[:n], addr)
	}
}
