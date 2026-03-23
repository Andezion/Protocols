#!/usr/bin/env bash

./server.sh &
SERVER_PID=$!
# Give the server a moment to start
sleep 1
# Connect to the TCP echo server and send a message
echo "Hello, TCP Echo Server!" | nc localhost 12345
# Clean up: kill the server and all its child processes
kill -- -$(ps -o pgid= -p $SERVER_PID | tr -d ' ') 2>/dev/null

