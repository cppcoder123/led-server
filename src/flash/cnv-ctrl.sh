#!/bin/bash

ROOT=/sys/class/gpio

ENABLE=27
RESET=24

GPIO_ENABLE=${ROOT}/gpio${ENABLE}
#GPIO_RESET=${ROOT}/gpio${RESET}

if [ $# == 0 ]; then
    echo "Please add argument: enable/disable"
    exit 1
fi

function open () {
    echo ${ENABLE} > ${ROOT}/export
    #
    echo out > ${GPIO_ENABLE}/direction
    echo 1 > ${GPIO_ENABLE}/value
}

function close () {
    # set reset high
    #echo ${RESET} > ${ROOT}/export
    #echo out > ${GPIO_RESET}/direction
    #echo 1 > ${GPIO_RESET}/value
    # release pins
    echo ${ENABLE} > ${ROOT}/unexport
    #echo ${RESET} > ${ROOT}/unexport
}

if [ $1 == "enable" ]; then
    open
elif [ $1 == "disable" ]; then
    close
else
    echo "Unknown argument, exiting ..."
    exit 2
fi
