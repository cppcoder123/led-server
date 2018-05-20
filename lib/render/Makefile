#
# Rendering library
#
TARGET = src/librender.a


HEADER_LIST = cout.hpp  uart.hpp  hw.hpp
OBJECT_LIST = uart.o

HEADER_SRC_LIST = $(foreach file, $(HEADER_LIST), include/$(file))
OBJECT_SRC_LIST = $(foreach file, $(OBJECT_LIST), src/$(file))

CPPFLAGS = $(COMMON_CPPFLAGS) -I src -I include -I $(LIB_CORE)

ALL: $(TARGET)

$(TARGET): $(OBJECT_SRC_LIST)
	ar rcs $@ $^

src/%.o: src/%.cpp
	echo $(LIB_CORE)
	gcc $(CPPFLAGS) -c $< -o $@

clean:
	-rm $(OBJECT_SRC_LIST) $(TARGET)

.PHONY: clean
