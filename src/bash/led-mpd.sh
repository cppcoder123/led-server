#!/bin/bash

echo "pid: "$$

trap 'echo "Caught SIGUSR1"' SIGUSR1

echo "sleeping ..."



while :
do
    sleep infinity &
    wait $!
    echo "sleep over"
done

echo "mpd: abc"
echo "mpd: def"
echo "mpd: ghi"
echo "mpd: jkl"
echo "mpd: mno"
echo "mpd: pqr"
echo "mpd: stu"
