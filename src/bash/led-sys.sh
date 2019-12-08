#!/bin/bash

echo "pid: "$$

PAUSE_IF_OK=1
trap 'PAUSE_IF_OK=0' SIGUSR1


prefix_echo () {
    echo "sys: "$MSG
}

MSG="Checking Network..."
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

MSG="Checking Mpd..."
prefix_echo
#
STATUS=1
while [ $STATUS -ne 0 ]
do
    sudo systemctl status mpd | grep running >& /dev/null
    STATUS=$?
    if [ $STATUS -ne 0 ]
    then
        sudo systemctl start mpd >& /dev/null
    fi
    if [ $STATUS -ne 0 ]
    then
        sleep 1
    fi
done
#
MSG="Mpd is OK"
prefix_echo

MSG="Connecting Bluetooth speaker..."
prefix_echo
#
STATUS=1
while [ $STATUS -ne 0 ]
do
    led-bluetooth.sh status | grep Connected >& /dev/null
    STATUS=$?
    if [ $STATUS -ne 0 ]
    then
        led-bluetooth.sh connect >& /dev/null
    fi
    if [ $STATUS -ne 0 ]
    then
        led-bluetooth.sh disconnect >& /dev/null
        sleep 5
    fi
done
#
MSG="Bluetooth speaker is connected"
prefix_echo

MSG="Enabling Mpd Output"
prefix_echo
#
STATUS=1
while [ $STATUS -ne 0 ]
do
    mpc outputs | grep enabled >& /dev/null
    STATUS=$?
    if [ $STATUS -ne 0 ]
    then
        mpc enable 1 >& /dev/null
    fi
    if [ $STATUS -ne 0 ]
    then
        sleep 1
    fi
done
#
MSG="Mpd Output is enabled"
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

