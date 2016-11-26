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
#COMPILE_FLAGS = -I$(INCLUDE_INT) -I$(INCLUDE_EXT) -std=c++11 -pedantic -g
#LINK_FLAGS = -lpthread

#
#
#
ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST)
	$(CXX) -o $@ $^ $(LDFLAGS) -lpthread


%.o: %.cpp $(HEADER_LIST)
	$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
