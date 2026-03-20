package main

import (
	"net"
)

func main() {
	listener, err := net.Listen("tcp", ":8090")
	if err != nil {
		panic(err)
		// log.Fatal(err)
	}
}
