SET (SOURCES
    main_UnitTest.cpp

	# Common files for all tests
	TestClasses.h
	TestClasses.cpp

	# AU library
	lib/au/SingletonTest.cpp
	lib/au/STest.cpp
	lib/au/DescriptorsTest.cpp
	lib/au/EnvironmentTest.cpp
	lib/au/CommandLineTest.cpp
	lib/au/RateTest.cpp
	lib/au/CounterCollectionTest.cpp
	lib/au/StringComponentsTest.cpp
	lib/au/stringTest.cpp

	# AU CONTAINERS LIBRARY
	lib/au/containers/SharedPointerTest.cpp
	lib/au/containers/DictionaryTest.cpp
	lib/au/containers/QueueTest.cpp
	lib/au/containers/BoxTest.cpp
	lib/au/containers/StringVectorTest.cpp

	# AU NETWORK LIBRARY
	lib/au/network/FileDescriptorTest.cpp
	lib/au/network/NetworkListenerTest.cpp
	lib/au/network/RESTServiceTest.cpp

	# AU STATISTICS LIBRARY
    lib/au/statistics/ActivityMonitorTest.cpp

	# AU MUTEX LIBRARY
    lib/au/mutex/TokenTest.cpp
    lib/au/mutex/TokenTest2.cpp

	# AU tables library
	lib/au/tables/TableTest.cpp	

	# ENGINE library
	lib/engine/statusTest.cpp
	lib/engine/EngineTest.cpp

	# SAMSON COMMON LIBRARY
	lib/samson/common/SamsonSetupTest.cpp 

	# SAMSOM WORKER library
	lib/samson/worker/SamsonWorkerTest.cpp


	#Common files
	common_engine_test.h
	common_engine_test.cpp

	#samson lib

	samson/common/SamsonFileTest.cpp

    #engine
    engine/object_test.cpp
    engine/engine_test.cpp
    engine/memoryManager_test.cpp
    engine/diskOperation_test.cpp
    engine/diskManager_test.cpp
	engine/processManager_test.cpp
    engine/processItem_test.cpp
    engine/buffer_test.cpp
    engine/readFile_test.cpp

	#samson_system lib
	samson_system/samson_system.cpp

	#comscore library
	comscore/comscore.cpp
	
	#common library
	common/common_test.cpp

	#delilah library
	delilah/common_delilah_test.h
	delilah/common_delilah_test.cpp
	delilah/delilahConsole_test.cpp
	
	#samson/client library
	samson/client/SamsonClient_test.cpp

	#samon/isolated library
	samson/isolated/SharedMemoryManager_test.cpp
)


find_path(GTEST_INCLUDE_DIR gtest/gtest.h
          PATHS /usr/includes 
          PATH_SUFFIXES gtest )

find_library(GTESTMAIN_LIBRARY gtest_main
             PATHS /usr/lib64 /usr/local/lib64)

SET (HEADERS
     ${GTEST_INCLUDE_DIR}
)

SET (STATIC_LIBS
     ${HAVE_GTEST}
     ${GTESTMAIN_LIBRARY}
     delilahLib
     pa
     lm
     engine
     protobuf
     pthread
     samsonNetwork
     samsonStream
     samsonModule
     samsonWorkerLib
     samsonCommon
	 samsonZoo
	 zookeeper_mt
	 samson_comscore
	 samson_system
	 samsonClient
	 samsonIsolated
     au
     xmlmarkup
     xmlparser	
     curses
     dl
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/apps")
include_directories("${PROJECT_SOURCE_DIR}/extern")
include_directories("/usr/include")

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(unitTest ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(unitTest ${STATIC_LIBS} )

#INSTALL(TARGETS               unitTest
#        RUNTIME DESTINATION   bin)
