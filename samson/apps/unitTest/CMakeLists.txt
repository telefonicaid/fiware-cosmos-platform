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
	lib/au/TokenizerTest.cpp

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

	# AU tables library
	lib/au/tables/TableTest.cpp	

	# ENGINE library
	lib/engine/statusTest.cpp
	lib/engine/EngineTest.cpp
	lib/engine/BufferTest.cpp
    lib/engine/DiskOperationTest.cpp        
	lib/engine/MemoryManagerTest.cpp        
	lib/engine/ProcessItemTest.cpp              
	lib/engine/ReadFileTest.cpp
	lib/engine/DiskManagerTest.cpp          
	lib/engine/EngineTest.cpp               
	lib/engine/NotificationListenerTest.cpp 
	lib/engine/NotificationListenersManagerTest.cpp
	lib/engine/ProcessManagerTest.cpp       
	lib/engine/statusTest.cpp
	lib/engine/SimpleBufferTest.cpp
	lib/engine/EngineElementCollectionTest.cpp
	lib/engine/NotificationTest.cpp
	lib/engine/NotificationElementTest.cpp
	lib/engine/EngineElementTest.cpp
	lib/engine/ReadFileManagerTest.cpp



	# SAMSON COMMON LIBRARY
	lib/samson/common/SamsonSetupTest.cpp 
	lib/samson/common/SamsonFileTest.cpp

	# SAMSOM WORKER library
	lib/samson/worker/SamsonWorkerTest.cpp

  #SAMSON SYSTEM library
  lib/samson_system/ValueTest.cpp

	#Common files
	common_engine_test.h
	common_engine_test.cpp

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
     samsonModule
     samsonWorkerLib
     samsonStream
     samsonNetwork
     samsonZoo
     samsonCommon
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
