#!/bin/bash

echo "pid: "$$

TRACK_NAME="Empty track"
echo_track () {
    echo "mpd: "$TRACK_NAME
}

trap echo_track SIGUSR1

GO_AHEAD=1
trap "GO_AHEAD=0" SIGUSR2

while [ $GO_AHEAD -eq 1 ]
do
    TRACK_NAME=`mpc current`
    #echo $TRACK_NAME
    mpc idle &
    wait $!
done

