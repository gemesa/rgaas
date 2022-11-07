#!/bin/bash

echo "valgrind-server starting..."
valgrind --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --verbose \
            --error-exitcode=1 \
            --log-file="$(dirname "$0")/../../build/valgrind-server.log" \
            "$(dirname "$0")/../../build/rgaas-server" > "$(dirname "$0")/../../build/rgaas-server.log" &

valgrind_pid=$!
echo "valgrind_pid: $valgrind_pid"

echo "waiting for valgrind results... (sleep 15)"
sleep 15

kill -INT $valgrind_pid
echo "SIGINT sent to valgrind_pid ($valgrind_pid)"

echo "waiting for valgrind to finish writing the log file..."
sleep 3

echo "valgrind-server.log:"
cat "$(dirname "$0")/../../build/valgrind-server.log"

echo "rgaas-server.log:"
cat "$(dirname "$0")/../../build/rgaas-server.log"

wait $valgrind_pid