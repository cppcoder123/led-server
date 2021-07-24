#!/bin/bash

SYSTEM_CTL="sudo systemctl"
APP="led"

${SYSTEM_CTL} status ${APP} | egrep "active \(running\)" >& /dev/null
if [ ! $? -eq 0 ]
then
    ${SYSTEM_CTL} stop ${APP}
    ${SYSTEM_CTL} start ${APP}
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
