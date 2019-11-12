#
# led-info-d daemon makefile
#

TARGET = led-info-d

OBJECT_LIST = led-info-d.o arg.o clock.o content.o daemon.o

#COMPILE_FLAGS = -I$(INCLUDE_INT) -I$(INCLUDE_EXT) -std=c++11 -Wall -pedantic -g
#LINK_FLAGS = $(LIB_DAEMON) -lpthread

CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_ASIO) -I $(LIB_DAEMON)/include \
  -I $(LIB_ROOT)

LDFLAGS = -L $(LIB_DAEMON)/src

ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST)
	$(CXX) -o $@ $^ $(LDFLAGS) -ldaemon -lpthread


%.o: %.cpp $(wildcard *.hpp)
	+$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-info-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) \
          Makefile *.hpp *.cpp
