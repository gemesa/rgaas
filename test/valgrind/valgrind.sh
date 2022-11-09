#!/bin/bash

echo "valgrind-server starting..."
valgrind --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --verbose \
  --error-exitcode=1 \
  --log-file="$(dirname "$0")/../../build/valgrind-server.log" \
  "$(dirname "$0")/../../build/rgaas-server" >"$(dirname "$0")/../../build/rgaas-server.log" &

valgrind_server_pid=$!

echo "valgrind_server_pid: $valgrind_server_pid"

echo "valgrind-client starting..."
valgrind --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --verbose \
  --error-exitcode=1 \
  --log-file="$(dirname "$0")/../../build/valgrind-client.log" \
  "$(dirname "$0")/../../build/rgaas-client" -t >"$(dirname "$0")/../../build/rgaas-client.log" &

valgrind_client_pid=$!

echo "valgrind_client_pid: $valgrind_client_pid"

echo "waiting for valgrind-server and valgrind-client results... (sleep 15)"
sleep 15

kill -INT $valgrind_server_pid
echo "SIGINT sent to valgrind_server_pid ($valgrind_server_pid)"

echo "waiting for valgrind-server and valgrind-client to finish writing the log file... (sleep 3)"
sleep 3

echo "valgrind-server.log:"
cat "$(dirname "$0")/../../build/valgrind-server.log"

echo "rgaas-server.log:"
cat "$(dirname "$0")/../../build/rgaas-server.log"

echo "valgrind-client.log:"
cat "$(dirname "$0")/../../build/valgrind-client.log"

echo "rgaas-client.log:"
cat "$(dirname "$0")/../../build/rgaas-client.log"

wait $valgrind_server_pid
server_code=$?

wait $valgrind_client_pid
client_code=$?

echo ""
echo "summary:"
if [ "$server_code" -ne 0 ]; then
  echo "valgrind-server failed"
  exit 1
elif [ "$client_code" -ne 0 ]; then
  echo "valgrind-client failed"
  exit 1
else
  echo "valgrind-server and valgrind-client passed"
  exit 0
fi
