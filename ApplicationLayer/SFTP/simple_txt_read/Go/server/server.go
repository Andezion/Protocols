package main

import (
	"log"

	"github.com/pkg/sftp"
	"golang.org/x/crypto/ssh"
)

func main() {
	var conn *ssh.Client

	client, err := sftp.NewClient(conn)
	if err != nil {
		log.Fatal(err)
	}
	defer client.Close()

	w := client.Walk("/home/user")
	for w.Step() {
		if w.Err() != nil {
			continue
		}
		log.Println(w.Path())
	}

	f, err := client.Create("/home/user/test.txt")
	if err != nil {
		log.Fatal(err)
	}

	if _, err := f.Write([]byte("Hello, SFTP!")); err != nil {
		log.Fatal(err)
	}
	f.Close()
}
