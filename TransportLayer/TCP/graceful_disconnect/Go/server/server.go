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
}
