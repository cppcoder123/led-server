#
#
#
TARGET = libledhw.a

HEADER_LIST = ../hw.hpp ../cout.hpp ../spi.hpp spi-io.hpp 
OBJECT_LIST = spi.o spi-io.o


COMPILE_FLAGS = -I./ -I$(INCLUDE_INT) -std=c++11 -Wall -pedantic -g
LINK_FLAGS = -lpthread

$(TARGET): $(OBJECT_LIST)
	ar rcs $@ $^

%.o: %.cpp $(HEADER_LIST)
	g++ $(COMPILE_FLAGS) -c $<

.PHONY: clean

clean:
	-rm $(OBJECT_LIST) $(TARGET)
