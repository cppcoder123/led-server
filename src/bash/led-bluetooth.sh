#!/bin/bash

# control bluetooth connection

CTRL="sudo bluetoothctl"
DISCONNECT="disconnect"
ID="FC:58:FA:0E:D5:3D"
POWER_ON="power on"


#echo "id :" $ID 

if [ $1 == $DISCONNECT ]
then
    echo $POWER_ON | $CTRL
    echo $DISCONNECT $ID | $CTRL
else
    echo $POWER_ON | $CTRL
    echo "connect "$ID | $CTRL
fi
