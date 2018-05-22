#
# led-d daemon makefile
#

TARGET = led-d

#
#
#
#
HEADER_LIST = arg.hpp daemon.hpp display.hpp font.hpp font-dir.hpp \
  hex-font.hpp idle-request.hpp message.hpp message-ptr.hpp queue.hpp \
  log-wrapper.hpp meta-queue.hpp network.hpp render.hpp session.hpp 

OBJECT_LIST = led-d.o arg.o daemon.o display.o font.o font-dir.o hex-font.o \
  log-wrapper.o network.o render.o session.o


#
#
#

#LIB_HW = $(ROOT_INT)/ledhw/src/libledhw.a

LDFLAGS = -L $(LIB_RENDER)/src -L $(LIB_DAEMON)/src -lrender -ldaemon -lpthread

#LDFLAGS += ldaemon

CPPFLAGS = $(COMMON_CPPFLAGS) -I $(LIB_CORE) -I $(LIB_RENDER)/include \
  -I $(LIB_DAEMON)/include -I $(LIB_ASIO) -I $(LIB_ROOT)

#
#
#
ALL: $(TARGET)

$(TARGET): $(OBJECT_LIST) $(LIB_RENDER)/src/librender.a $(LIB_CORE)/*
	$(CXX) -o $@ $(OBJECT_LIST) $(LDFLAGS)


%.o: %.cpp $(HEADER_LIST)
	$(CXX) $(CPPFLAGS) -c $<


.PHONY: clean tar

clean:
	-rm $(OBJECT_LIST) $(TARGET)

tar:
	tar cfvz led-d.tar.gz $(ROOT_EXTERNAL) $(ROOT_INTERNAL) Makefile *.hpp *.cpp
