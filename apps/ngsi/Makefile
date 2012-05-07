INCLUDE		= -I/usr/include/libxml2
CFLAGS		= $(INCLUDE) -g -Wall
LIBS		= -lxml2
CPP		    = g++
OBJS        = registerContextOld.o discoverContextAvailability.o ws.o xmlParse.o xmlParseTest.o registerContext.o
EXEC        = xmlParseTest


all:        $(EXEC)

$(EXEC):	$(OBJS)
			$(CPP) -o $(EXEC) $(OBJS) $(LIBS)

clean:
			\rm -f $(OBJS) $(EXEC)

%.o: %.cpp
			$(CPP) $(CFLAGS) -c $^ -o $@
