package main

import "net"

func main() {
	addr := "127.0.0.1:8090"
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		panic(err)
		// log.Fatalf("Failed to connect to %s: %v", addr, err)
	}

	defer conn.Close()

	// клиент кричит
	message := "Hello from client\n"
	_, err = conn.Write([]byte(message))
	if err != nil {
		panic(err)
		// log.Fatalf("Failed to send message: %v", err)
	}

	// слушаем что кричат в ответ
	buf := make([]byte, 1024)
	n, err := conn.Read(buf)
	if err != nil {
		panic(err)
		// fmt.Fprintln(os.Stderr, "Error reading response:", err)
		// return
	}

	println("Received from server:", string(buf[:n]))

	println("Client finished one handshake and will exit.")
}
