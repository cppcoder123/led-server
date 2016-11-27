#
#
#

ROOT = $(shell pwd)

ROOT_EXT = $(ROOT)/lib/ext
export ROOT_INT = $(ROOT)/lib/int

export INCLUDE_EXT = $(ROOT_EXT)/include
export INCLUDE_INT = $(ROOT_INT)

export LIB_DAEMON = $(ROOT_EXT)/lib/libdaemon.a

CPPFLAGS ?= -std=c++11 -Wall -pedantic -g \
	-I$(INCLUDE_INT) -I$(INCLUDE_EXT)

LDFLAGS ?= -L$(ROOT_EXT)/lib

#
#
#
LIB_LEDHW = $(ROOT_INT)/ledhw/src/libledhw.a

LED_D = led-d/led-d
LED_INFO_D = led-info-d/led-info-d 
LED = led/led

TARGET_LIST = $(LED_D) $(LED_INFO_D) $(LED)

ALL: $(TARGET_LIST)

led%: $(LIB_LEDHW)
	cd $(dir $@) && \
	make CPPFLAGS="$(CPPFLAGS)" LDFLAGS="$(LDFLAGS)" && \
	cd $(ROOT)

$(LIB_LEDHW):
	cd $(dir $@) && \
	make CPPFLAGS="$(CPPFLAGS)" \
	&& cd $(ROOT)

install:
	cp led-d/led-d $(DESTDIR)
	cp led-info-d/led-info-d $(DESTDIR)
	cp led/led $(DESTDIR)

clean:
	-rm $(wildcard $(dir $(LED_D))*.o)
	-rm $(wildcard $(dir $(LED_INFO_D))*.o)
	-rm $(wildcard $(dir $(LED))*.o)
	-rm $(wildcard $(dir $(LIB_LEDHW))*.o)
	-rm $(TARGET_LIST) $(LIB_LEDHW)

.PHONY: clean
