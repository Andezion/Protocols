package main

import (
	"bufio"
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
