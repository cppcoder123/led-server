#
# led-d daemon makefile
#

TARGET = led-d

#
#
#
#
HEADER_LIST = arg.hpp daemon.hpp display.hpp font.hpp font-dir.hpp \
  hex-font.hpp idle-request.hpp message.hpp message-ptr.hpp queue.hpp \
  log-wrapper.hpp meta-queue.hpp network.hpp render.hpp session.hpp 

OBJECT_LIST = led-d.o arg.o daemon.o display.o font.o font-dir.o hex-font.o \
  log-wrapper.o network.o render.o session.o


#
#
#
COMPILE_FLAGS = -I$(INCLUDE_INT) -I$(INCLUDE_EXT) -std=c++11 -Wall -pedantic -g

LINK_FLAGS = $(LIB_DAEMON) -lpthread

#
#
#
ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST)
	g++ -o $@ $^ $(LINK_FLAGS)


%.o: %.cpp $(HEADER_LIST)
	g++ $(COMPILE_FLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
