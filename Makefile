#
# Top level makefile
#

ROOT = $(shell pwd)

export LIB_ROOT = $(ROOT)/lib
export LIB_ASIO = $(LIB_ROOT)/asio
export LIB_ARDUINO = $(LIB_ROOT)/arduino
export LIB_CORE = $(LIB_ROOT)/core
export LIB_DAEMON = $(LIB_ROOT)/daemon
export LIB_RENDER = $(LIB_ROOT)/render
export LIB_TCLAP = $(LIB_ROOT)/tclap

export COMMON_CPPFLAGS = -std=c++17 -Wall -pedantic -g

SERVER = $(ROOT)/server
INFO_GATEWAY = $(ROOT)/info-gateway
TEST_CLIENT = $(ROOT)/test-client

LIB_TO_BUILD = $(LIB_DAEMON) $(LIB_RENDER)
APP_TO_BUILD = $(TEST_CLIENT) $(INFO_GATEWAY) $(SERVER)

# lib(s) first
TARGET_PATH_LIST = $(LIB_TO_BUILD) $(APP_TO_BUILD)

ALL: $(TARGET_PATH_LIST)

$(TARGET_PATH_LIST):
	cd $@ && make && cd $(ROOT)

install:
	-echo "Implement me"
#cp led-d/led-d $(DESTDIR)
#cp led-info-d/led-info-d $(DESTDIR)
#cp led/led $(DESTDIR)

clean:
	-echo 'Implement me'
#-rm $(wildcard $(dir $(LED_D))*.o)
#-rm $(wildcard $(dir $(LED_INFO_D))*.o)
#-rm $(wildcard $(dir $(LED))*.o)
#-rm $(wildcard $(dir $(LIB_LEDHW))*.o)
#-rm $(TARGET_LIST) $(LIB_LEDHW)

.PHONY: clean $(LIB_TO_BUILD) $(APP_TO_BUILD)
