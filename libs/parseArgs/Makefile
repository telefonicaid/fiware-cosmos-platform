#
# FILE		parseArgs:makefile - makefile for the parse argument package
#
# AUTHOR	Ken Zangelin
#
#
TEST_LIBS       =	-L$(TIP_HOME)/lib -lba -llm
INCLUDE		=	-I. -I$(TIP_HOME)/include
DFLAGS		=	-DANSI -DLM_ON -g -Wall
CFLAGS		=	-fPIC $(DFLAGS) $(INCLUDE)
CC		=	gcc

PA_LIB		=	libpa.a
PA_LIB_SHARED	=	libpa.so
PKG_CONFIG_FILE =	pa.pc

OBJS    	=	paBuiltin.o         \
			paConfig.o          \
			paDefaultValues.o   \
			paEnvVals.o         \
			paFrom.o            \
			paFullName.o        \
			paGetVal.o          \
			paIsOption.o        \
			paIsSet.o           \
			paIterate.o         \
			paIxLookup.o        \
			paLimitCheck.o      \
			paLogSetup.o        \
			paOptions.o         \
			paOptionsParse.o    \
			paParse.o           \
			paRcFile.o          \
			paUsage.o           \
			paValueFrom.o       \
			paWarning.o

HEADERS		=	paBuiltin.h                \
			paConfig.h                 \
			paDefaultValues.h          \
			paEnvVals.h                \
			paFrom.h                   \
			paFullName.h               \
			paGetVal.h                 \
			paIsOption.h               \
			paIsSet.h                  \
			paIterate.h                \
			paIxLookup.h               \
			paLimitCheck.h             \
			paLog.h                    \
			paLogSetup.h               \
			paOptions.h                \
			paOptionsParse.h           \
			paParse.h                  \
			paPrivate.h                \
			paRcFile.h                 \
			paTraceLevels.h            \
			paUsage.h                  \
			paValueFrom.h              \
			paWarning.h                \
			parseArgs.h


all:			$(PA_LIB) $(PA_LIB_SHARED) pcgen

install:		all
			cp $(PA_LIB)          $(TIP_HOME)/lib/
			cp $(PA_LIB_SHARED)   $(TIP_HOME)/lib/
			cp $(HEADERS)         $(TIP_HOME)/include/
			cp $(PKG_CONFIG_FILE) $(TIP_HOME)/lib/pkgconfig

release:		install

clean:;			\rm -f $(PA_LIB) $(PA_LIB_SHARED) $(OBJS)

pcgen:
			echo "prefix=$(TIP_HOME)"   >   $(PKG_CONFIG_FILE)
			cat $(PKG_CONFIG_FILE).in  >>   $(PKG_CONFIG_FILE)

uninstall:
			rm -f $(TIP_HOME)/lib/$(PA_LIB)
			rm -f $(TIP_HOME)/lib/$(PA_LIB_SHARED)
			rm -f $(TIP_HOME)/lib/pkgconfig/$(PKG_CONFIG_FILE)
			(cd $(TIP_HOME)/include; rm -f $(HEADERS))


$(PA_LIB):		$(OBJS)
			ar rv $(PA_LIB) $(OBJS)
			ranlib $(PA_LIB)

$(PA_LIB_SHARED):	$(OBJS)
			gcc -shared -o $(PA_LIB_SHARED) $(OBJS)

parseTests:		parseTest parseTest2 parseTest3

parseTest:		$(PA_LIB) parseTest.o
			$(CC) -o parseTest parseTest.o $(PA_LIB) $(TEST_LIBS)

parseTest.o:		parseTest.c parseArgs.h
			$(CC) -c $(CFLAGS) parseTest.c

parseTest2:		$(PA_LIB) parseTest2.o
			$(CC) -o parseTest2 parseTest2.o $(PA_LIB) $(TEST_LIBS)

parseTest2.o:		parseTest2.c parseArgs.h
			$(CC) -c $(CFLAGS) parseTest2.c

parseTest3:		$(PA_LIB) parseTest3.o
			$(CC) -o parseTest3 parseTest3.o $(PA_LIB) $(TEST_LIBS)

parseTest3.o:		parseTest3.c parseArgs.h
			$(CC) -c $(CFLAGS) parseTest3.c
