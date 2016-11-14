#
# Compile code for atmega8a
#
TARGET = atmega8a.hex

MCU = atmega8a
#F_CPU = 16000000UL

#PROGRAMMER_TYPE = avrdude
#PROGRAMMER_ARGS =

#CC = avr-gcc
#OBJCOPY = avr-objcopy
#OBJDUMP = avr-objdump
#AVRSIZE = avr-size
#AVRDUDE = avrdude

CFLAGS_OPT = -I../../ -mmcu=$(MCU) -Os -std=gnu99 -Wall 
LDFLAGS_OPT = -mmcu=$(MCU)

CFLAGS_DEBUG = $(CFLAGS) -g
LDFLAGS_DEBUG = $(LDFLAGS) -g

CFLAGS = $(CFLAGS_OPT)
LDFLAGS = $(LDFLAGS_OPT)

ALL: $(TARGET)

$(TARGET): atmega8a.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size $@

atmega8a.elf: atmega8a.o
	avr-gcc $(LDFLAGS) -o $@ $<

atmega8a.o: atmega8a.c
	avr-gcc $(CFLAGS) -c $<

clean:
	-rm *.o *.elf *.hex $(TARGET)

.PHONY: clean
