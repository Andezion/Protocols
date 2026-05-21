package main

import (
	"fmt"
	"os"
	"os/exec"
)

func main() {
	fmt.Println("Enter 1 to send POST request to /echo, 2 to send GET request to /ping:")

	for {
		var output int
		fmt.Scan(&output)
		switch output {
		case 1:
			cmd := exec.Command("curl", "http://localhost:8080/echo", "-X", "POST", "-d", "Hello, Echo Server!\n")
			cmd.Stdout = os.Stdout

			if err := cmd.Run(); err != nil {
				panic(err)
			}
		case 2:
			cmd := exec.Command("curl", "http://localhost:8080/ping")
			cmd.Stdout = os.Stdout

			if err := cmd.Run(); err != nil {
				panic(err)
			}
		}
	}
}
