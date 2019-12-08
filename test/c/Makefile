#
#
#

TARGET=buffer-test

BUFFER_SRC = ../../src/avr/buffer.c
RING_SRC = ../../src/avr/ring.c
BUFFER_TEST_SRC = buffer-test.c

BUFFER_O=buffer.o
RING_O=ring.o
BUFFER_TEST_O=buffer-test.o

OBJECT_LIST = $(BUFFER_O) $(RING_O) $(BUFFER_TEST_O)

INC=-I ../../src/avr -I ../../src -I .
GPP_CMD=g++ -c -g $(INC)

$(TARGET) : $(OBJECT_LIST)
	g++ -o $@ $(OBJECT_LIST)

$(BUFFER_O): $(BUFFER_SRC)
	$(GPP_CMD) $^
$(RING_O): $(RING_SRC)
	$(GPP_CMD) $^
$(BUFFER_TEST_O): $(BUFFER_TEST_SRC)
	$(GPP_CMD) $^

clean:
	-rm $(OBJECT_LIST) $(TARGET)

.PHONY: clean
