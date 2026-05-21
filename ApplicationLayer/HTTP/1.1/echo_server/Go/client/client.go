package main

import (
	"os"
	"os/exec"
)

func main() {

	cmd := exec.Command("echo", "Hello User, to test echo server click 1, to test ping server click 2")
	cmd.Stdout = os.Stdout

	if err := cmd.Run(); err != nil {
		panic(err)
	}

	// cmd := exec.Command("curl", "http://localhost:8080/echo", "-X", "POST", "-d", "Hello, Echo Server!\n")
	// cmd.Stdout = os.Stdout

	// if err := cmd.Run(); err != nil {
	// 	panic(err)
	// }

}
