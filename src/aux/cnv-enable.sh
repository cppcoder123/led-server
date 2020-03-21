#!/bin/bash

ROOT=/sys/class/gpio

echo 24 > ${ROOT}/export 
sleep 1
echo out > ${ROOT}/gpio24/direction 
sleep 1
echo 1 > ${ROOT}/gpio24/value 
sleep 1
