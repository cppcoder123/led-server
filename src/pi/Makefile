#
# led-d daemon makefile
#

TARGET = led-d

ROOT = $(shell pwd)

LIB_ROOT = $(ROOT)/..
LIB_UTIL = $(LIB_ROOT)/util

COMMON_CPPFLAGS = -DASIO_STANDALONE -std=c++17 -Wall -pedantic -g

#
OBJECT_LIST = led-d.o arg.o base.o bash.o command.o content.o \
  font.o handle.o mcu-block.o mcu-handle.o mcu-parse.o \
  popen.o render.o spi-dev.o spi-enable.o spi-interrupt.o \
  status.o

#
#
#
LDFLAGS = -lgpiod -lpthread

CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_ROOT)

#
#
#
ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST) $(LIB_UTIL)/*
	$(CXX) -o $@ $(OBJECT_LIST) $(LDFLAGS)

%.o: %.cpp $(wildcard *.hpp)
	+$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
