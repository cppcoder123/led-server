# Internet radio (+ clock) with a LED matrix as a display and several control knobs

Hardware part of the project consists of the Raspberry Pi Zero W and Arduino Mega Pro.
Raspberry Pi drives MPD daemon and provides a text stream for Arduino and
Arduino handles the display and control knobs.

Raspberry establishes Wifi connection to Home network and bluetooth connection
to bluetooth speaker.
When radio doesn't work display shows current time, i.e. Raspberry is switched off,
while arduino works 24/7.
