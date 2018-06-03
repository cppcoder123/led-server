#
# Top level makefile
#

ROOT = $(shell pwd)

export LIB_ROOT = $(ROOT)/lib
export LIB_ASIO = $(LIB_ROOT)/asio
export LIB_ARDUINO = $(LIB_ROOT)/arduino
export LIB_CORE = $(LIB_ROOT)/core
export LIB_DAEMON = $(LIB_ROOT)/daemon
export LIB_DEVICE = $(LIB_ROOT)/device
export LIB_TCLAP = $(LIB_ROOT)/tclap

export COMMON_CPPFLAGS = -std=c++17 -Wall -pedantic -g

SERVER = $(ROOT)/server
INFO_GATEWAY = $(ROOT)/info-gateway
TEST_CLIENT = $(ROOT)/test-client

# lib(s) first
TARGET_PATH_LIST = $(LIB_DAEMON) $(LIB_DEVICE) $(TEST_CLIENT) $(INFO_GATEWAY) $(SERVER)

ALL: $(TARGET_PATH_LIST)

$(TARGET_PATH_LIST):
	cd $@ && make && cd $(ROOT)

clean:
	$(foreach dir, $(TARGET_PATH_LIST), cd $(dir) && make clean;)
	-find . -iname "*~" | xargs rm -f

.PHONY: clean $(TARGET_PATH_LIST)
