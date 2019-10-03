#
# led-d daemon makefile
#

TARGET = led-d

#
#
#
#

OBJECT_LIST = led-d.o arg.o block.o daemon.o font-dir.o font.o handle.o \
  mcu.o network.o parse.o render.o session.o spi-dev.o spi-irq.o

#
#
#

LDFLAGS = -L $(LIB_DAEMON)/src -ldaemon -lgpiod -lpthread


GCC_VERSION := $(shell g++ -dumpversion)
ifeq ($(GCC_VERSION), 6.3.0)
OPTIONAL_DEFINE := -DGCC6
endif

CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_DAEMON)/include \
  -I $(LIB_ASIO) -I $(LIB_ROOT) $(OPTIONAL_DEFINE)

#
#
#
ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST) $(LIB_CORE)/*
	$(CXX) -o $@ $(OBJECT_LIST) $(LDFLAGS)


%.o: %.cpp $(wildcard *.hpp)
	+$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
