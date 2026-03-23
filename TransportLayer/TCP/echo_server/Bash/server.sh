#!/usr/bin/env bash

echo "Starting TCP echo server on port 12345..."
while true; do
    # Listen for incoming connections on port 12345
    nc -l -p 12345 -c 'cat' || sleep 1
done

    