#
# led-d daemon makefile
#

TARGET = led-d

#
#
#
#

OBJECT_LIST = arg.o block.o content.o daemon.o font-dir.o font.o handle.o \
  hex-font.o idle-request.o led-d.o log-wrapper.o network.o render.o \
  session.o spi.o spi-gpio.o spi-parse.o


#
#
#

LDFLAGS = -L $(LIB_DAEMON)/src -ldaemon -lgpiod -lpthread

CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_DAEMON)/include \
  -I $(LIB_ASIO) -I $(LIB_ROOT)

#
#
#
ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST) $(LIB_CORE)/*
	$(CXX) -o $@ $(OBJECT_LIST) $(LDFLAGS)


%.o: %.cpp $(wildcard *.hpp)
	$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
