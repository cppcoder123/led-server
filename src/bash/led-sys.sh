#!/bin/bash

echo "pid: "$$

PAUSE_IF_OK=1
trap 'PAUSE_IF_OK=0' SIGUSR1


prefix_echo () {
    echo "sys: "$MSG
}

MSG="Checking network"
prefix_echo
#
STATUS=1
while [ $STATUS -ne 0 ]
do
    ip link | grep wlan| grep UP >& /dev/null
    STATUS=$?
    if [ $STATUS -ne 0 ]
    then
        sleep 1
    fi
done
#
MSG="Network is OK"
prefix_echo

MSG="Checking mpd"
prefix_echo
#
STATUS=1
while [ $STATUS -ne 0 ]
do
    sudo ps -alef | grep mpd >& /dev/null
    STATUS=$?
    if [ $STATUS -ne 0 ]
    then
        sudo mpd
    fi
    if [ $STATUS -ne 0 ]
    then
        sleep 1
    fi
done
#
MSG="Mpd is OK"
prefix_echo

MSG="Connecting bluetooth speaker"
prefix_echo
#
STATUS=1
while [ $STATUS -ne 0 ]
do
    led-bluetooth.sh status | grep Connected >& /dev/null
    STATUS=$?
    if [ $STATUS -ne 0 ]
    then
        led-bluetooth.sh connect
    fi
    if [ $STATUS -ne 0 ]
    then
        led-bluetooth.sh disconnect
        sleep 5
    fi
done
#
MSG="Bluetooth speaker is connected"
prefix_echo

MSG="Enabling mpd output"
prefix_echo
#
STATUS=1
while [ $STATUS -ne 0 ]
do
    mpc outputs | grep enabled >& /dev/null
    STATUS=$?
    if [ $STATUS -ne 0 ]
    then
        mpc enable 1
    fi
    if [ $STATUS -ne 0 ]
    then
        sleep 1
    fi
done
#
MSG="Mpd output is enabled"
prefix_echo

#
# Hang here
#

while [ $PAUSE_IF_OK -ge 1 ]
do
    sleep infinity &
    wait $!
done

#
#
#

MSG="Stopping the system"
prefix_echo

#led-bluetooth.sh disconnect

