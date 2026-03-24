#!/usr/bin/env bash

{ echo "Hello from Client"; sleep 1; } | nc localhost 12345

