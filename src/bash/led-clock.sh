#!/bin/bash

echo "pid: "$$

GO_AHEAD=1

trap 'GO_AHEAD=0' SIGUSR1

while [ $GO_AHEAD -ne 0 ]
do
    date "+%H:%M %A %e-%B"
    sleep 60
done
