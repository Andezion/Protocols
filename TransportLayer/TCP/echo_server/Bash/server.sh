#!/usr/bin/env bash

echo "Starting TCP echo server on port 12345..."
while true; do
    nc -l -p 12345 -c 'while IFS= read -r line; do echo "Client says: $line" >&2; echo "Hello from server!"; done'
done

    