#
# led-d daemon makefile
#

TARGET = led-test-client

#
#
#
HEADER_LIST = arg.hpp 

OBJECT_LIST = led.o arg.o 


#
#
#
CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_CORE) -I $(LIB_ASIO) -I $(LIB_ROOT)


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

#tar:
#tar cfvz led.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
