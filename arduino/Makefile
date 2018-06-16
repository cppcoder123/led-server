#
# Compile code for atmega8a
#
TARGET = arduino.hex

OBJECT_LIST = \
  arduino.o \
  button.o \
  codec.o \
  queue.o \
  matrix-buffer.o \
  parse.o \
  render.o \
  self-test.o \
  spi-write.o \
  uart.o \
  uart-read.o \
  uart-write.o
#OBJECT_LIST = \
  arduino.o \
  codec.o \
  queue.o \
  parse.o \
  uart.o \
  uart-read.o \
  uart-write.o

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

arduino.elf: $(OBJECT_LIST)
	avr-gcc $(LDFLAGS) -o $@ $^


%.o: %.c $(wildcard *.h)
	avr-gcc $(CFLAGS) -c $<

clean:
	-rm *~ *.o *.elf *.hex $(TARGET)

.PHONY: clean
