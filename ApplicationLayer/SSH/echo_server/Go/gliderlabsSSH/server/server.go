package main

import (
	"io"
	"log"

	"github.com/gliderlabs/ssh"
)

func main() {
	ssh.Handle(func(s ssh.Session) {
		io.WriteString(s, "Welcome to the echo server!\n")
	})

	log.Fatal(ssh.ListenAndServe(":8080", nil))
}
