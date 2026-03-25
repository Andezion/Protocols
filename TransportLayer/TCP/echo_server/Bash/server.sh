#!/usr/bin/env bash

echo "Starting TCP echo server on port 12345..."
# Бесконечный цикл для обработки входящих соединений
while true; do
    # Используем netcat для прослушивания порта 12345 и обработки входящих сообщений
    # -l - слушать входящие соединения
    # -p 12345 - указать порт для прослушивания
    # -c - выполнить команду для каждого входящего соединения
    #  while IFS= read -r line; do ... done - цикл для чтения входящих сообщений от клиента
    nc -l -p 12345 -c 'while IFS= read -r line; do echo "Client says: $line" >&2; echo "Hello from server!"; done'
done

    