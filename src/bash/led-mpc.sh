#!/bin/bash

# invoke mpc

get_track_id () {
    TRACK_ID=`mpc -f %position% current`
}

case $1 in
    play)
        mpc play
        ;;
    track-get)
        get_track_id
        echo $TRACK_ID
        ;;
    volume-get)
        mpc volume | sed 's/volume: //; s/%//'
        ;;
    track-set)
        max=`mpc playlist | wc -l`
        get_track_id
        if [ $2 == "+" ]
        then
            new_track_id=$(($TRACK_ID + $3))
            if ((new_track_id > max))
            then
                arg=$max
            else
                arg=$(($TRACK_ID + $3))
            fi
        else
            if (($TRACK_ID > $3))
            then
                arg=$(($TRACK_ID - $3))
            else
                arg="1"
            fi
        fi
        #echo "arg= " $arg
        mpc play $arg
        ;;
    volume-set)
        arg=$2$3
        #echo "arg " $arg
        #mpc volume $arg >& /dev/null
        mpc volume $arg
        ;;
    *)
        echo "Unknown action"
        exit 1
esac
