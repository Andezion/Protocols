package main

import (
	"fmt"
	"log"
	"net"

	"yggdrasil-mini/yggmini"
)

// client2 - дозванивающийся узел
func main() {
	node, err := yggmini.NewNode()
	if err != nil {
		log.Fatalf("[client2] NewNode: %v", err)
	}
	fmt.Printf("[client2] identity ready, my Yggdrasil address: %s\n", yggmini.AddrString(node.Addr))

	fmt.Println("[client2] connecting to client1 on 127.0.0.1:9001...")
	conn, err := net.Dial("tcp", "127.0.0.1:9001")
	if err != nil {
		log.Fatalf("[client2] dial: %v", err)
	}
	defer conn.Close()
	fmt.Println("[client2] connected, exchanging metadata...")

	peerPub, err := yggmini.RecvMetadata(conn)
	if err != nil {
		log.Fatalf("[client2] recv metadata: %v", err)
	}
	if err := yggmini.SendMetadata(conn, node); err != nil {
		log.Fatalf("[client2] send metadata: %v", err)
	}

	peerAddr := yggmini.AddrForKey(peerPub)
	fmt.Printf("[client2] peer identity confirmed, peer Yggdrasil address: %s\n", yggmini.AddrString(peerAddr))

	peerBoxPub, err := yggmini.BoxPubForKey(peerPub)
	if err != nil {
		log.Fatalf("[client2] BoxPubForKey: %v", err)
	}

	msg, err := yggmini.RecvEncrypted(conn, node, &peerBoxPub)
	if err != nil {
		log.Fatalf("[client2] recv encrypted: %v", err)
	}
	fmt.Printf("[client2] received decrypted: %s\n", msg)

	reply := []byte("hello back from client2, this reached you encrypted too")
	if err := yggmini.SendEncrypted(conn, node, &peerBoxPub, reply); err != nil {
		log.Fatalf("[client2] send encrypted: %v", err)
	}
	fmt.Printf("[client2] sent encrypted: %s\n", reply)
}
