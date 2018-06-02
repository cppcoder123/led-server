#
# Compile code for atmega8a
#
TARGET = arduino.hex

MCU = atmega328p
#F_CPU = 16000000UL

#PROGRAMMER_TYPE = avrdude
#PROGRAMMER_ARGS =

#CC = avr-gcc
#OBJCOPY = avr-objcopy
#OBJDUMP = avr-objdump
#AVRSIZE = avr-size
#AVRDUDE = avrdude

CFLAGS_OPT = -I ../lib/core -mmcu=$(MCU) -Os -std=gnu99 -Wall 
LDFLAGS_OPT = -mmcu=$(MCU)

CFLAGS_DEBUG = $(CFLAGS) -g
LDFLAGS_DEBUG = $(LDFLAGS) -g

CFLAGS = $(CFLAGS_OPT)
LDFLAGS = $(LDFLAGS_OPT)

ALL: $(TARGET)

$(TARGET): arduino.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size $@

arduino.elf: arduino.o codec.o parse.o
	avr-gcc $(LDFLAGS) -o $@ $^


arduino.o: arduino.c
	avr-gcc $(CFLAGS) -c $<

codec.o: codec.c
	avr-gcc $(CFLAGS) -c $<

parse.o: parse.c
	avr-gcc $(CFLAGS) -c $<


clean:
	-rm *~ *.o *.elf *.hex $(TARGET)

.PHONY: clean
