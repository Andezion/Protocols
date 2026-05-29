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

	fmt.Println("Started echo server on port 8090...")
}
