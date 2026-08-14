package main

import (
	"fmt"
	"log"
	"net"

	"yggdrasil-mini/yggmini"
)

// client1 - принимающий узел сначала слушаем, потом кто-то к нам приходит
func main() {
	node, err := yggmini.NewNode()
	if err != nil {
		log.Fatalf("[client1] NewNode: %v", err)
	}
	fmt.Printf("[client1] identity ready, my Yggdrasil address: %s\n", yggmini.AddrString(node.Addr))

	ln, err := net.Listen("tcp", ":9001")
	if err != nil {
		log.Fatalf("[client1] listen: %v", err)
	}
	defer ln.Close()
	fmt.Println("[client1] waiting for a peer on TCP port 9001...")

	conn, err := ln.Accept()
	if err != nil {
		log.Fatalf("[client1] accept: %v", err)
	}
	defer conn.Close()
	fmt.Println("[client1] peer connected, exchanging metadata...")

	if err := yggmini.SendMetadata(conn, node); err != nil {
		log.Fatalf("[client1] send metadata: %v", err)
	}
	peerPub, err := yggmini.RecvMetadata(conn)
	if err != nil {
		log.Fatalf("[client1] recv metadata: %v", err)
	}

	peerAddr := yggmini.AddrForKey(peerPub)
	fmt.Printf("[client1] peer identity confirmed, peer Yggdrasil address: %s\n", yggmini.AddrString(peerAddr))

	peerBoxPub, err := yggmini.BoxPubForKey(peerPub)
	if err != nil {
		log.Fatalf("[client1] BoxPubForKey: %v", err)
	}

	hello := []byte("hello from client1, over an encrypted link")
	if err := yggmini.SendEncrypted(conn, node, &peerBoxPub, hello); err != nil {
		log.Fatalf("[client1] send encrypted: %v", err)
	}
	fmt.Printf("[client1] sent encrypted: %s\n", hello)

	reply, err := yggmini.RecvEncrypted(conn, node, &peerBoxPub)
	if err != nil {
		log.Fatalf("[client1] recv encrypted: %v", err)
	}
	fmt.Printf("[client1] received decrypted: %s\n", reply)
}
