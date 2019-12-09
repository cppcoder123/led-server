#!/bin/bash

echo "pid: "$$

# fixme
# exit 0

TRACK_NAME="No-track-name"
echo_track () {
    echo "mpd: "${TRACK_NAME}
}

GO_AHEAD=1

#trap echo_track SIGUSR1
#trap "echo ZZZ" SIGUSR1
trap "GO_AHEAD=0" SIGUSR1

NAME=`mpc current` >& /dev/null
STATUS=$?
if [ "${STATUS}" == 0 ] && [ "${NAME}" != "" ]
then
    TRACK_NAME=${NAME}
fi

echo_track

while [ "${GO_AHEAD}" == 1 ]
do
    NAME=`mpc current` >& /dev/null
    STATUS=$?
    if [ "${STATUS}" == 0 ]
    then
        mpc idle
    else
        sleep 5
    fi
    echo_track
    #echo $TRACK_NAME
    #mpc idle >& /dev/null >& /dev/null &
    #wait $!
done

