#
# led-d daemon makefile
#

TARGET = led-d

ROOT = $(shell pwd)

LIB_ROOT = $(ROOT)/..
LIB_ASIO = $(LIB_ROOT)/asio
LIB_TCLAP = $(LIB_ROOT)/tclap

COMMON_CFLAGS = -DVARRUN=\"/var/run/led\"
COMMON_CPPFLAGS = -DASIO_STANDALONE -std=c++17 -Wall -pedantic -g

#
# font-dir.o 
# bash-in.o
OBJECT_LIST = led-d.o arg.o base.o bash.o command.o content.o \
  font.o handle.o mcu-block.o mcu-handle.o mcu-parse.o \
  popen.o render.o spi-dev.o spi-interrupt.o spi-open.o \
  status.o

#
#
#
LDFLAGS = -lgpiod -lpthread

CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_ASIO) -I $(LIB_ROOT) $(OPTIONAL_DEFINE)

#
#
#
ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST) $(LIB_UNIX)/*
	$(CXX) -o $@ $(OBJECT_LIST) $(LDFLAGS)

%.o: %.cpp $(wildcard *.hpp)
	+$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
