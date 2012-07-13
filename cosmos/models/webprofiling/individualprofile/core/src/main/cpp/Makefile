COMSCOREDIR=../../../../../../../../third-party/comscore-dictionary
CXXFLAGS=-shared -fpic
LDDFLAGS=-I/usr/java/default/include -I$(COMSCOREDIR) \
	 -I/usr/java/default/include/linux \
	 -I/usr/lib/jvm/java-6-sun/include \
	 -I/usr/lib/jvm/java-6-sun/include/linux \
	 -I/System/Library/Frameworks/JavaVM.framework/Headers

all: libCategories.so

es/tid/cosmos/profile/dictionary/comscore/CSDictionaryJNIInterface.class:
	javac -d . ../java/es/tid/cosmos/profile/dictionary/comscore/NativeCSDictionary.java
	javac -d . ../java/es/tid/cosmos/profile/dictionary/comscore/CSDictionaryJNIInterface.java

es_tid_cosmos_profile_dictionary_comscore_CSDictionaryJNIInterface.h: es/tid/cosmos/profile/dictionary/comscore/CSDictionaryJNIInterface.class
	javah -jni es.tid.cosmos.profile.dictionary.comscore.CSDictionaryJNIInterface

libCategories.so: es_tid_cosmos_profile_dictionary_comscore_CSDictionaryJNIInterface.h es_tid_cosmos_profile_dictionary_comscore_CSDictionaryJNIInterface.cpp
	g++ $(CXXFLAGS) $(LDDFLAGS) -o libCategories.so es_tid_cosmos_profile_dictionary_comscore_CSDictionaryJNIInterface.cpp $(COMSCOREDIR)/libcsCFD.so.1.0

clean:
	rm -rf es es_tid_cosmos_profile_dictionary_comscore_CSDictionaryJNIInterface.h libCategories.so

.PHONY: clean all
