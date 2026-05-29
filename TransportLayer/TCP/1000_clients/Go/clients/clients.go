package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
)

type Client struct {
	name string,
	id int,
}