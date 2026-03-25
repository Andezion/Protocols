#!/usr/bin/env bash

# nc localhost - подключаемся к серверу на localhost
{ echo "Hello from Client"; sleep 1; } | nc localhost 12345

