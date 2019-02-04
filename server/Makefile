#
# led-d daemon makefile
#

TARGET = led-d

#
#
#
#

#OBJECT_LIST = led-d.o arg.o block.o content.o daemon.o font.o font-dir.o hex-font.o \
  log-wrapper.o idle-request.o initial.o network.o pipe.o render.o serial.o session.o

OBJECT_LIST = arg.o block.o daemon.o font-dir.o font.o hex-font.o led-d.o log-wrapper.o \
  spi.o


#
#
#

LDFLAGS = -L $(LIB_DAEMON)/src -ldaemon -lpthread

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
