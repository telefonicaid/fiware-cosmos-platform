CC           = gcc
CPP          = g++
INCLUDE      = 
DFLAGS       = 
CFLAGS       = -O2 -fPIC -Wall -Wno-unused-function $(DFLAGS) $(INCLUDE)

EXEC1        = arcanumTunnel
EXEC2        = sniffer
EXEC3        = arcanumTestServer

OBJS1        = arcanumTunnel.o
OBJS2        = sniffer.o
OBJS3        = arcanumTestServer.o

OS           = $(shell uname)
ifeq ($(OS),SunOS)
  LIBS       = -lsocket -lc -lnsl
endif


all:		 $(EXEC1) $(EXEC2) $(EXEC3)

clean:
			\rm $(EXEC1) $(EXEC2) $(EXEC3) $(OBJS1) $(OBJS2) $(OBJS3)

$(EXEC1):	$(OBJS1)
			$(CPP) $(CFLAGS) -o $@ $(OBJS1) $(LIBS)

$(EXEC2):	$(OBJS2)
			$(CPP) $(CFLAGS) -o $@ $(OBJS2) $(LIBS)

$(EXEC3):	$(OBJS3)
			$(CPP) $(CFLAGS) -o $@ $(OBJS3) $(LIBS)

%.o: %.c
		$(CC) $(CFLAGS) -c $^ -o $@
