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
