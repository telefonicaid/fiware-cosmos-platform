CC           = gcc
CPP          = g++
INCLUDE      = -I../../libs/
DFLAGS       = -Wall -DOS=$(OS) -g
CFLAGS       = $(INCLUDE) $(DFLAGS) -O2 -fPIC -Wall -Wno-unused-function $(DFLAGS) $(INCLUDE)

EXEC1        = tektronixTunnel
EXEC2        = sniffer
EXEC3        = tektronixTestServer
EXEC4        = tektronix
OBJS1        = tektronixTunnel.o
OBJS2        = sniffer.o
OBJS3        = tektronixTestServer.o
OBJS4        = tektronix.o

ARCHIVE      = tektronixTunnelTools.tar

OS           = $(shell uname)

ifeq ($(OS),SunOS)
  LIBS       = -lsocket -lc -lnsl
endif

.PHONY 	     = clean package

all:		$(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4)

clean:
			rm -f $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4) $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(ARCHIVE).gz

$(EXEC1):	$(OBJS1)
			$(CPP) -o $@ $(OBJS1) $(LIBS)

$(EXEC2):	$(OBJS2)
			$(CPP) -o $@ $(OBJS2) $(LIBS)

$(EXEC3):	$(OBJS3)
			$(CPP) -o $@ $(OBJS3) $(LIBS)

$(EXEC4):	$(OBJS4)
			$(CPP) -o $@ $(OBJS4) $(LIBS)

tektronix.o:  tektronix.cpp
			$(CPP) $(CFLAGS) -c $^ -o $@

%.o: %.cpp
		$(CPP) $(CFLAGS) -c $^ -o $@

%.o: %.c
		$(CC) $(CFLAGS) -c $^ -o $@

package: $(EXEC1) $(EXEC2) $(EXEC4)
			tar -cf $(ARCHIVE) $(EXEC1) $(EXEC2) $(EXEC4) $(EXEC1).c $(EXEC2).c $(EXEC4).cpp Makefile README.md
			gzip -f $(ARCHIVE)

# vim: ts=4 sw=4 noexpandtab
