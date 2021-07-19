#!/bin/bash

IS_RUNNING="0"
status () {
    sudo systemctl status led | egrep "active \(running\)" >& /dev/null
    IS_RUNNING=$?
}

status ()

if [! ${IS_RUNNING} -eq 0]
then
    sudo systemctl stop led
    sudo systemctl start led
fi

status ()
if [! ${IS_RUNNING} -eq 0]
then
    exit
fi

# check bt connection
echo "info" | sudo bluetoothctl | grep "Connected: yes" >& /dev/null
if [ ! $? -eq 0 ]
then
    echo "connect FC:58:FA:0E:D5:3D" | sudo bluetoothctl >& /dev/null
fi

# check audio stream
echo "mpc status" | grep playing >& /dev/null
if [ ! $? -eq 0 ]
then
    mpc play
fi
