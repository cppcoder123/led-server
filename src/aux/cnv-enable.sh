#!/bin/bash

ROOT=/sys/class/gpio

echo 22 > ${ROOT}/export 
sleep 1
echo out > ${ROOT}/gpio22/direction 
sleep 1
echo 1 > ${ROOT}/gpio22/value 
sleep 1
