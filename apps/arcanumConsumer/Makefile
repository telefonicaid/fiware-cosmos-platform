CC           = gcc
CPP          = g++
INCLUDE      = -I../../libs/
DFLAGS       = 
CFLAGS       = $(INCLUDE) $(DFLAGS) -O2 -fPIC -Wall -Wno-unused-function $(DFLAGS) $(INCLUDE)

EXEC1        = arcanumTunnel
EXEC2        = sniffer
EXEC3        = arcanumTestServer
EXEC4        = arcanum
OBJS1        = arcanumTunnel.o
OBJS2        = sniffer.o
OBJS3        = arcanumTestServer.o
OBJS4        = arcanum.o

OS           = $(shell uname)

ifeq ($(OS),SunOS)
  LIBS       = -lsocket -lc -lnsl
endif


all:		 $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4)

clean:
			\rm $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4) $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4)

$(EXEC1):	$(OBJS1)
			$(CPP) -o $@ $(OBJS1) $(LIBS)

$(EXEC2):	$(OBJS2)
			$(CPP) -o $@ $(OBJS2) $(LIBS)

$(EXEC3):	$(OBJS3)
			$(CPP) -o $@ $(OBJS3) $(LIBS)

$(EXEC4):	$(OBJS4)
			$(CPP) -o $@ $(OBJS4) $(LIBS)

arcanum.o:  arcanum.cpp
			$(CPP) $(CFLAGS) -c $^ -o $@

%.o: %.cpp
		$(CPP) $(CFLAGS) -c $^ -o $@

%.o: %.c
		$(CC) $(CFLAGS) -c $^ -o $@

