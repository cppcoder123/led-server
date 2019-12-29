#!/bin/bash

echo "pid: "$$

PAUSE_IF_OK=1
trap 'PAUSE_IF_OK=0' SIGUSR1


#prefix_echo () {
#    echo "sys: "$MSG
#}

echo "Checking Network..."
#prefix_echo
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
echo "Network is OK"
#prefix_echo

echo "Checking Mpd..."
#prefix_echo
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
echo "Mpd is OK"
#prefix_echo

echo "Connecting Bluetooth speaker..."
#prefix_echo
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
echo "Bluetooth speaker is connected"
#prefix_echo

echo "Enabling Mpd Output"
#prefix_echo
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
echo "Mpd Output is enabled"
#prefix_echo

mpc status | grep playing >& /dev/null
if [ $? -ne 0 ]
then
   echo "mpc play: "
fi

echo "mpc playlist: "

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

echo "Stopping the system"
#prefix_echo

#led-bluetooth.sh disconnect

