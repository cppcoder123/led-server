#
#
#

ROOT = $(shell pwd)

ROOT_EXT = $(ROOT)/lib/ext
ROOT_INT = $(ROOT)/lib/int

export INCLUDE_EXT = $(ROOT_EXT)/include
export INCLUDE_INT = $(ROOT_INT)

export LIB_DAEMON = $(ROOT_EXT)/lib/libdaemon.a

#
#
#
EXE_LIST = led-d led-info-d led

ALL: $(EXE_LIST)
	for exe in $(EXE_LIST); do \
		cd $$exe && make && cd .. ; \
	done

clean:
	for exe in $(EXE_LIST); do \
		cd $$exe && make clean && cd ..; \
	done

tar:
	tar cfvz led-all.tar.gz Makefile led led-d led-info-d lib/int

.PHONY: clean tar
