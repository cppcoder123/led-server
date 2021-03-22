#!/bin/bash

# invoke mpc

get_track_id () {
    TRACK_ID=`mpc -f %position% current`
}

get_track_num ()
{
    TRACK_NUM=`mpc playlist | wc -l`
}

case $1 in
    play)
        mpc play
        ;;
    playlist-get)
        max=`mpc lsplaylists | wc -l`
        if [ "$max" -eq 0 ]
        then
            echo "0-0-0"
        else
            echo "0-1-"$max
        fi
        ;;
    playlist-name)
        if [ ! -z $2 ]
        then
            name=`mpc lsplaylists | sed "$2q;d"`
            if [ -n "$name" ]
            then
                echo "$2-"$name
            else
                echo "$2-n/a"
            fi
        fi
        ;;
    playlist-set)
        if [ $2 -ne 0 ]
        then
            name=`mpc lsplaylists | sed "$2q;d"`
            if [ -n "$name" ]
            then
                mpc stop
                mpc clear
                mpc load $name
                mpc play 1
            fi
        fi
        ;;
    stop)
        mpc stop
        ;;
    track-get)
        get_track_id
        get_track_num
        if [ -z "$TRACK_ID" ]
        then
            echo "0-0-0"
        else
            echo $TRACK_ID"-"1"-"$TRACK_NUM
        fi
        ;;
    track-name)
        if [ ! -z $2 ]
        then
            name=`mpc -f %file% playlist | sed "$2q;d"`
            if [ -n "$name" ]
            then
                echo "$2-"$name
            else
                echo "$2-n/a"
            fi
        fi
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
    volume-get)
        VOLUME_LEVEL=`mpc volume | sed 's/volume://; s/%//'`
        echo $VOLUME_LEVEL"-"0"-"100
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
