#
#
#

ROOT = $(shell pwd)

ROOT_EXT = $(ROOT)/lib/ext
export ROOT_INT = $(ROOT)/lib/int

export INCLUDE_EXT = $(ROOT_EXT)/include
export INCLUDE_INT = $(ROOT_INT)

export LIB_DAEMON = $(ROOT_EXT)/lib/libdaemon.a


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
	cd $(dir $@) && make && cd $(ROOT)

$(LIB_LEDHW):
	cd $(dir $@) && make && cd $(ROOT)

clean:
	-rm $(wildcard $(dir $(LED_D))*.o)
	-rm $(wildcard $(dir $(LED_INFO_D))*.o)
	-rm $(wildcard $(dir $(LED))*.o)
	-rm $(wildcard $(dir $(LIB_LEDHW))*.o)
	-rm $(TARGET_LIST) $(LIB_LEDHW)

.PHONY: clean
