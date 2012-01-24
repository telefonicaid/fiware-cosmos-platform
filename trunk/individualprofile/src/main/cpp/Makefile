COMSCOREDIR=../../../../../third-party/comscore-dictionary
CXXFLAGS=-shared -fpic
LDDFLAGS=-I/usr/java/default/include -I$(COMSCOREDIR) \
	 -I/usr/java/default/include/linux -I/System/Library/Frameworks/JavaVM.framework/Headers

all: libCategories.so

es/tid/ps/profile/dictionary/CSDictionaryJNIInterface.class:
	javac -d . ../java/es/tid/ps/profile/dictionary/CSDictionaryJNIInterface.java

es_tid_ps_profile_dictionary_CSDictionaryJNIInterface.h: es/tid/ps/profile/dictionary/CSDictionaryJNIInterface.class
	javah -jni es.tid.ps.profile.dictionary.CSDictionaryJNIInterface

libCategories.so: es_tid_ps_profile_dictionary_CSDictionaryJNIInterface.h es_tid_ps_profile_dictionary_CSDictionaryJNIInterface.cpp
	g++ $(CXXFLAGS) $(LDDFLAGS) -o libCategories.so es_tid_ps_profile_dictionary_CSDictionaryJNIInterface.cpp $(COMSCOREDIR)/libcsCFD.so.1.0

clean:
	rm -rf es es_tid_ps_profile_dictionary_CSDictionaryJNIInterface.h libCategories.so

.PHONY: clean all
