#
# led-d daemon makefile
#

TARGET = led

#
#
#
HEADER_LIST = arg.hpp 

OBJECT_LIST = led.o arg.o 


#
#
#
COMPILE_FLAGS = -I$(INCLUDE_INT) -I$(INCLUDE_EXT) -std=c++11 -pedantic -g

LINK_FLAGS = -lpthread

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
	tar cfvz led.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
