#!/bin/bash

remove="output/*"
supervisor_output="output/supervisor_servers.txt"
clients_output="output/clients.txt"

echo "testing..."

rm -f $remove
./executable/supervisor 8 >> $supervisor_output &
supervisor_pid=$!

sleep 2

for (( i=1; i<=10; i++ )); do
    ./executable/client 8 5 20 >> $clients_output &
    ./executable/client 8 5 20 >> $clients_output &
    sleep 1
done

for (( i=1; i<=6; i++ )); do
    sleep 10
    kill -SIGINT $supervisor_pid
done

result=0
while [ $result -eq 0 ]; do
    sleep 0.5
    kill -SIGINT $supervisor_pid
    result=$!
done

wait $supervisor_pid

./misura.sh $supervisor_output $clients_output

echo "done."