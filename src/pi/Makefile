#
# led-d daemon makefile
#

TARGET = led-d

ROOT = $(shell pwd)

LIB_ROOT = $(ROOT)/..
LIB_ASIO = $(LIB_ROOT)/asio
LIB_DAEMON = $(LIB_ROOT)/daemon
LIB_TCLAP = $(LIB_ROOT)/tclap

COMMON_CFLAGS = -DVARRUN=\"/var/run/led\"
COMMON_CPPFLAGS = -DASIO_STANDALONE -std=c++17 -Wall -pedantic -g

#
# Dependency
#
LIB_DAEMON_OBJECT = $(LIB_DAEMON)/src/libdaemon.a
LIB_PATH_LIST = $(LIB_DAEMON)

#
#
#
OBJECT_LIST = led-d.o arg.o base.o bash-handle.o bash.o font-dir.o font.o \
  mcu-block.o mcu-handle.o mcu-parse.o render.o session.o spi-dev.o \
  spi-irq.o spi-open.o

#
#
#
LDFLAGS = -L $(LIB_DAEMON)/src -ldaemon -lgpiod -lpthread

CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_DAEMON)/include \
  -I $(LIB_ASIO) -I $(LIB_ROOT) $(OPTIONAL_DEFINE)

#
#
#
ALL: $(TARGET)

$(TARGET): $(LIB_DAEMON_OBJECT) $(OBJECT_LIST) $(LIB_UNIX)/*
	$(CXX) -o $@ $(OBJECT_LIST) $(LDFLAGS)

$(LIB_DAEMON_OBJECT):
	cd $(LIB_DAEMON) && make

#$(TARGET): $(OBJECT_LIST) $(LIB_UNIX)/*


%.o: %.cpp $(wildcard *.hpp)
	+$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-cd $(LIB_DAEMON) && make clean
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
