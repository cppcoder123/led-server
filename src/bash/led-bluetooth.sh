#!/bin/bash

# control bluetooth connection

CTRL="sudo bluetoothctl"
DISCONNECT="disconnect"
CONNECT="connect"
ID="FC:58:FA:0E:D5:3D"
POWER_ON="power on"


#echo "id :" $ID 

if [ $1 == $DISCONNECT ]
then
    echo $POWER_ON | $CTRL
    echo $DISCONNECT $ID | $CTRL
elif [ $1 == $CONNECT ]
then
    echo $POWER_ON | $CTRL
    echo $CONNECT $ID | $CTRL
else
    echo $POWER_ON | $CTRL
    echo "info" $ID | $CTRL | grep "Connected: yes"
    if [ $? -eq 0 ]
    then
        echo "Connected"
    else
        echo "Disconnected"
    fi
fi
