#
# FILE		parseArgs:makefile - makefile for parse argument test programs
#
# AUTHOR	Ken Zangelin
#
#
SAMSON_HOME         = /opt/samson
LIBS                = -L$(SAMSON_HOME)/lib -llm -lpa
INCLUDE             = -I. -I.. -I$(SAMSON_HOME)/include
DFLAGS              = -DANSI -DLM_ON -g -Wall
CFLAGS              = -fPIC $(DFLAGS) $(INCLUDE)
CC                  = g++


all:				parseTests

parseTests:			parseTest parseTest2 parseTest3

parseTest:			$(PA_LIB) parseTest.o
					$(CC) -o parseTest parseTest.o $(PA_LIB) $(LIBS)

parseTest.o:		parseTest.cpp parseArgs.h
					$(CC) -c $(CFLAGS) parseTest.cpp

parseTest2:			$(PA_LIB) parseTest2.o
					$(CC) -o parseTest2 parseTest2.o $(PA_LIB) $(LIBS)

parseTest2.o:		parseTest2.cpp parseArgs.h
					$(CC) -c $(CFLAGS) parseTest2.cpp

parseTest3:			$(PA_LIB) parseTest3.o
					$(CC) -o parseTest3 parseTest3.o $(PA_LIB) $(LIBS)

parseTest3.o:		parseTest3.cpp parseArgs.h
					$(CC) -c $(CFLAGS) parseTest3.cpp
