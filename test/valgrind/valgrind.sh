#!/bin/bash

echo "valgrind starting..."
valgrind --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --verbose \
            --error-exitcode=1 \
            --log-file="$(dirname "$0")/../../build/valgrind.log" \
            "$(dirname "$0")/../../build/rgaas" > "$(dirname "$0")/../../build/rgaas.log" &

valgrind_pid=$!
echo "valgrind_pid: $valgrind_pid"

echo "waiting for valgrind results... (sleep 15)"
sleep 15

kill -INT $valgrind_pid
echo "SIGINT sent to valgrind_pid ($valgrind_pid)"

echo "waiting for valgrind to finish writing the log file..."
sleep 3

echo "valgrind log:"
cat "$(dirname "$0")/../../build/valgrind.log"

echo "rgaas log:"
cat "$(dirname "$0")/../../build/rgaas.log"

wait $valgrind_pid