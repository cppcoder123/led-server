#
# led-d daemon makefile
#

TARGET = src/libdaemon.a

#
#
#
HEADER_LIST = daemon.h dexec.h dfork.h dlog.h dnonblock.h dpid.h dsignal.h
OBJECT_LIST = dexec.o dfork.o dlog.o dnonblock.o dpid.o dsignal.o

HEADER_SRC_LIST = $(foreach file, $(HEADER_LIST), include/$(file))
OBJECT_SRC_LIST = $(foreach file, $(OBJECT_LIST), src/$(file))

CFLAGS = -I include -g

#
#
#
#LDFLAGS += ldaemon

#
#
#
ALL: $(TARGET)

$(TARGET): $(OBJECT_SRC_LIST)
	ar rcs $@ $^


src/%.o: src/%.c $(HEADER_SRC_LIST)
	+gcc $(COMMON_CFLAGS) $(CFLAGS) -c $< -o $@


.PHONY: clean

clean:
	-rm $(OBJECT_SRC_LIST) $(TARGET)
