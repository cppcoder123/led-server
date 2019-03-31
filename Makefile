#
# Top level makefile
#

ROOT = $(shell pwd)

export LIB_ROOT = $(ROOT)/lib
export LIB_ASIO = $(LIB_ROOT)/asio
export LIB_PATH = $(LIB_ROOT)
export LIB_DAEMON = $(LIB_ROOT)/daemon
export LIB_TCLAP = $(LIB_ROOT)/tclap

export COMMON_CFLAGS = -DVARRUN=\"/var/run/led-d\"
export COMMON_CPPFLAGS = -DASIO_STANDALONE -std=c++17 -Wall -pedantic -g

SERVER = $(ROOT)/server
#INFO_GATEWAY = $(ROOT)/info-gateway
GATEWAY = $(ROOT)/gateway
#TEST_CLIENT = $(ROOT)/test-client

LIB_DAEMON_OBJECT = $(LIB_DAEMON)/src/libdaemon.a

LIB_PATH_LIST = $(LIB_DAEMON)
TARGET_PATH_LIST = $(GATEWAY) $(SERVER)

ALL: $(TARGET_PATH_LIST)

$(TARGET_PATH_LIST): $(LIB_DAEMON_OBJECT)
	+cd $@ && make && cd $(ROOT)

$(LIB_DAEMON_OBJECT):
	cd $(LIB_DAEMON) && make && cd $(ROOT)

clean:
	$(foreach dir, $(TARGET_PATH_LIST), cd $(dir) && make clean;)
	$(foreach dir, $(LIB_PATH_LIST), cd $(dir) && make clean;)
	-find . -iname "*~" | xargs rm -f

.PHONY: clean $(TARGET_PATH_LIST)
