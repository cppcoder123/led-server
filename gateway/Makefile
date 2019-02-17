#
# led-info-d daemon makefile
#

TARGET = led-info-d


HEADER_LIST = arg.hpp clock.hpp daemon.hpp delay.hpp init.hpp log-wrapper.hpp

OBJECT_LIST = led-info-d.o arg.o clock.o daemon.o init.o log-wrapper.o


#COMPILE_FLAGS = -I$(INCLUDE_INT) -I$(INCLUDE_EXT) -std=c++11 -Wall -pedantic -g
#LINK_FLAGS = $(LIB_DAEMON) -lpthread

CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_ASIO) -I $(LIB_DAEMON)/include \
  -I $(LIB_ROOT)

LDFLAGS = -L $(LIB_DAEMON)/src

ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST)
	$(CXX) -o $@ $^ $(LDFLAGS) -ldaemon -lpthread


%.o: %.cpp $(HEADER_LIST)
	$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-info-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) \
          Makefile *.hpp *.cpp
