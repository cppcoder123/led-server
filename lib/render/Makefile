#
# Rendering library
#
TARGET = src/librender.a


HEADER_LIST = cout.hpp  ht1632c.hpp  hw.hpp  spi.hpp spi-io.hpp
OBJECT_LIST = ht1632c.o  spi-io.o  spi.o  window.o

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
