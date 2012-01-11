CPP=g++
FLAGS=-Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DCSCFD_BUILD -fPIC
#Uncomment below line to enable database loading support
#FLAGS=-Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DCSCFD_BUILD -DENABLE_LINUX_DATABASE_LOAD -fPIC
INCLUDES=-I./ -I/usr/include

.cpp.o:
	@rm -f $@
	$(CPP) $(FLAGS) $(INCLUDES) -c -o $@ $<

CSCFDOBJECTS=Interface.o CFDMain.o CFDBase.o CFDManager.o CFDTree.o c_membuffer.o Logger_Linux.o Utils.o UtilString.o TermsInDomain.o CFDWildcard.o

all: csCFD

csCFD: ${CSCFDOBJECTS}
	$(CPP) -shared -Wl,-soname,libcsCFD.so.1 -o libcsCFD.so.1.0 *.o -lz -lpthread
	ar rcs libcsCFD.a *.o

clean:
	rm -rf *.o libcsCFD.a libcsCFD.so.1.0
# DO NOT DELETE
