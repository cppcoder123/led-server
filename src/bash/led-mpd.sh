#!/bin/bash

echo "pid: "$$

# fixme
exit 0

TRACK_NAME="Empty track"
echo_track () {
    echo "mpd: "$TRACK_NAME
}

GO_AHEAD=1
trap echo_track SIGUSR1
trap "GO_AHEAD=0" SIGUSR2

while [ $GO_AHEAD -eq 1 ]
do
    TRACK_NAME=`mpc current`
    STATUS=$?
    if [ $STATUS -ne 0 ]
    then
        sleep 3
    fi
    #echo $TRACK_NAME
    mpc idle >& /dev/null >& /dev/null &
    wait $!
done

