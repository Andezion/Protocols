package main

import (
	"bufio"
	"fmt"
	"net"
)

type Client struct {
	name   string
	id     int
	conn   net.Conn
	reader *bufio.Reader
	writer *bufio.Writer
}

func NewClient(name string, id int, conn net.Conn) *Client {
	return &Client{
		name:   name,
		id:     id,
		conn:   conn,
		reader: bufio.NewReader(conn),
		writer: bufio.NewWriter(conn),
	}
}

func (c *Client) SendMessage(message string) error {
	_, err := c.writer.WriteString(message + "\n")
	if err != nil {
		return err
	}
	return c.writer.Flush()
}

func (c *Client) ReadMessage() (string, error) {
	return c.reader.ReadString('\n')
}

func (c *Client) Close() error {
	return c.conn.Close()
}

func main() {
	addr := "127.0.0.1:8090"
	for i := 0; i < 1000; i++ {
		conn, err := net.Dial("tcp", addr)
		if err != nil {
			panic(err)
		}

		client := NewClient(fmt.Sprintf("Client%d", i), i, conn)
		go func(c *Client) {
			defer c.Close()
			c.SendMessage("Hello from " + c.name)
			response, err := c.ReadMessage()
			if err != nil {
				fmt.Printf("%s: Error reading response: %v\n", c.name, err)
				return
			}
			fmt.Printf("%s received: %s", c.name, response)
		}(client)
	}
}
