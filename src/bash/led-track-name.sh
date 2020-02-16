#!/bin/bash

echo "pid: "$$


TRACK_NAME="No-track-name"
TRACK_POSITION="0"
OLD_INFO=""

get_info () {
    TRACK_NAME=`mpc current` >& /dev/null
    if [ $? == 0 ]
    then
        TRACK_POSITION=`mpc -f %position% current` >& /dev/null
    fi
    if [ $? == 0 ]
    then
        TRACK_INFO=`echo "Channel "$TRACK_POSITION" "$TRACK_NAME`
    fi
}

#echo_info () {
#    echo $TRACK_INFO
#    NEW_INFO=`echo "Channel "$TRACK_POSITION" "$TRACK_NAME`
#    if [ "$NEW_INFO" != "$OLD_INFO" ]
#    then
#        echo $NEW_INFO
#        OLD_INFO=$NEW_INFO
#    fi
#}

GO_AHEAD=1

trap "GO_AHEAD=0" SIGUSR1

OLD_INFO=""

while [ "${GO_AHEAD}" == 1 ]
do
    get_info
    if [ $? == 0 ]
    then
        if [ "${OLD_INFO}" != "${TRACK_INFO}" ]
        then
            echo $TRACK_INFO
            OLD_INFO=$TRACK_INFO
        fi
        mpc idle >& /dev/null
    else
        sleep 5
    fi
    #echo_track
    #echo $TRACK_NAME
    #mpc idle >& /dev/null >& /dev/null &
    #wait $!
done

