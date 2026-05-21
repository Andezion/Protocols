package main

import (
	"os"
	"os/exec"
)

func main() {
	cmd := exec.Command("curl", "http://localhost:8080/echo", "-X", "POST", "-d", "Hello, Echo Server!\n")
	cmd.Stdout = os.Stdout

	if err := cmd.Run(); err != nil {
		panic(err)
	}
}
