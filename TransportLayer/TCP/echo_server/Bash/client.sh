#!/usr/bin/env bash

./server.sh &
SERVER_PID=$!
# Give the server a moment to start
sleep 1
# Connect to the TCP echo server and send a message
echo "Hello, TCP Echo Server!" | nc localhost 12345
# Clean up: kill the server process
kill $SERVER_PID

