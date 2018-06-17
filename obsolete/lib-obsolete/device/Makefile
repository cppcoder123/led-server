#
# Rendering library
#
TARGET = src/libdevice.a

OBJECT_LIST = uart.o

OBJECT_SRC_LIST = $(foreach file, $(OBJECT_LIST), src/$(file))

CPPFLAGS = $(COMMON_CPPFLAGS) -I src -I include -I $(LIB_CORE)

ALL: $(TARGET)

$(TARGET): $(OBJECT_SRC_LIST)
	ar rcs $@ $^

src/%.o: src/%.cpp $(wildcard include/*.hpp)
	echo $(LIB_CORE)
	gcc $(CPPFLAGS) -c $< -o $@

clean:
	-rm $(OBJECT_SRC_LIST) $(TARGET)

.PHONY: clean
