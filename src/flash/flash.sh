#!/bin/bash

sudo ./cnv-ctrl.sh enable 
sudo avrdude -c linuxspi -P /dev/spidev0.0 -v -p m2560 -U flash:w:../avr/avr.hex
#sudo ./cnv-ctrl.sh enable
sudo ./cnv-ctrl.sh disable
