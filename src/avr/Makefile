#
# Compile code for atmel-2560
#
TARGET = avr
TARGET_HEX = $(TARGET).hex
TARGET_ELF = $(TARGET).elf

#  buzz.o
#  render.o
OBJECT_LIST = \
  avr.o \
  buf.o \
  buffer.o \
  buzz.o \
  clock.o \
  counter.o \
  debug.o \
  decode.o \
  display.o \
  encode.o \
  flush.o \
  font.o \
  invoke.o \
  postpone.o \
  power.o \
  ring.o \
  rotor.o \
  spi.o \
  sync.o

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
