#
# Compile code for atmel-2560
#
TARGET = avr
TARGET_HEX = $(TARGET).hex
TARGET_ELF = $(TARGET).elf

OBJECT_LIST = \
  at.o \
  avr.o \
  boot.o \
  buf.o \
  buffer.o \
  counter.o \
  cron.o \
  debug.o \
  decode.o \
  display.o \
  encode.o \
  flush.o \
  font.o \
  heartbeat.o \
  menu.o \
  mode.o \
  postpone.o \
  render.o \
  ring.o \
  rotor.o \
  spi.o \
  twi.o \
  watch.o

MCU = atmega2560

CFLAGS_OPT = -I .. -mmcu=$(MCU) -Os -std=gnu99 -Wall 
LDFLAGS_OPT = -mmcu=$(MCU)

CFLAGS_DEBUG = $(CFLAGS) -g
LDFLAGS_DEBUG = $(LDFLAGS) -g

CFLAGS = $(CFLAGS_OPT)
LDFLAGS = $(LDFLAGS_OPT)

ALL: $(TARGET_HEX)

$(TARGET_HEX): $(TARGET_ELF)
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size $@

$(TARGET_ELF): $(OBJECT_LIST)
	avr-gcc $(LDFLAGS) -o $@ $^

%.o: %.c $(wildcard *.h)
	avr-gcc $(CFLAGS) -c $<

clean:
	-rm *~ *.o $(TARGET_ELF) $(TARGET_HEX)

.PHONY: clean
