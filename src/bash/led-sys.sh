#!/bin/bash

echo "pid: "$$

PAUSE_IF_OK=1
trap 'PAUSE_IF_OK=0' SIGUSR1

prefix_echo () {
    echo "sys: "$MSG
}

network_status () {
    ip link | grep wlan | grep UP
}

echo "do a"
echo "do b"
echo "do c"

while [ $PAUSE_IF_OK -ge 1 ]
do
    sleep infinity &
    wait $!
    echo "sleep over"
    echo $PAUSE_IF_OK
done

echo "do d"
echo "do e"
echo "do f"
