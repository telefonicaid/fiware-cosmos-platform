SET (SOURCES

    main_UnitTest.cpp

	# Common files for all tests
	TestClasses.h
	TestClasses.cpp

	# -----------------------------------------------------------
	# AU library
	# -----------------------------------------------------------


	lib/au/CommandLineTest.cpp
	lib/au/EnvironmentTest.cpp
	lib/au/fileTest.cpp

	# lib/au/string
	
	lib/au/string/DescriptorsTest.cpp
	lib/au/string/STest.cpp
	lib/au/string/StringCollectionTest.cpp
	lib/au/string/StringComponentsTest.cpp
	lib/au/string/StringUtilitiesTest.cpp
	lib/au/string/TokenizerTest.cpp
	lib/au/string/PatternTest.cpp


	# AU SINGLETON 

	lib/au/singleton/SingletonTest.cpp

	# AU CONTAINERS LIBRARY
	lib/au/containers/BoxTest.cpp
	lib/au/containers/DictionaryTest.cpp
	lib/au/containers/GlobalDictionaryTest.cpp
	lib/au/containers/SharedPointerTest.cpp
	lib/au/containers/StringVectorTest.cpp
	lib/au/containers/QueueTest.cpp

	# AU NETWORK LIBRARY
	lib/au/network/FileDescriptorTest.cpp
	lib/au/network/NetworkListenerTest.cpp
	lib/au/network/RESTServiceTest.cpp
	lib/au/network/ConsoleServiceTest.cpp

	# AU STATISTICS LIBRARY
    lib/au/statistics/ActivityMonitorTest.cpp
	lib/au/statistics/CounterCollectionTest.cpp
	lib/au/statistics/CronometerTest.cpp
	lib/au/statistics/RateTest.cpp

	# AU MUTEX LIBRARY
    lib/au/mutex/TokenTest.cpp

	# AU setup
	lib/au/setup/SetupItemCollectionTest.cpp
	lib/au/setup/SetupItemTest.cpp

	# AU tables library
	lib/au/tables/TableTest.cpp	

	# -----------------------------------------------------------
	# ENGINE library
	# -----------------------------------------------------------

	# Common files
	common_engine_test.h
	common_engine_test.cpp

	lib/engine/EngineTest.cpp
	lib/engine/BufferTest.cpp
	lib/engine/DiskManagerTest.cpp          
    lib/engine/DiskOperationTest.cpp        
	lib/engine/EngineElementCollectionTest.cpp
	lib/engine/EngineElementTest.cpp
	lib/engine/EngineTest.cpp               
	lib/engine/MemoryManagerTest.cpp        
	lib/engine/NotificationTest.cpp
	lib/engine/NotificationElementTest.cpp
	lib/engine/NotificationListenersManagerTest.cpp
	lib/engine/NotificationListenerTest.cpp 
	lib/engine/ProcessItemTest.cpp              
	lib/engine/ProcessManagerTest.cpp       
	lib/engine/ReadFileTest.cpp
	lib/engine/statusTest.cpp
	lib/engine/SimpleBufferTest.cpp
	lib/engine/ReadFileManagerTest.cpp

	# -----------------------------------------------------------
	# ETXRA libraries
	# -----------------------------------------------------------

	# EXTRA C4 LIBRARY
	lib/extra/c4/BoardTest.cpp 

	# EXTRA COMSCORE LIBRAY
	lib/extra/comscore/comscore.cpp

    # -----------------------------------------------------------
    # SAMSON libraries
    # -----------------------------------------------------------

	# SAMSON COMMON LIBRARY
	lib/samson/common/FullKVInfoTest.cpp
	lib/samson/common/KVHeaderTest.cpp
	lib/samson/common/KVInfoTest.cpp
	lib/samson/common/KVRangeDivisionTest.cpp
	lib/samson/common/KVRangeTest.cpp
	lib/samson/common/SamsonSetupTest.cpp 
	lib/samson/common/SamsonFileTest.cpp
	
	lib/samson/common/EnvironmentOperationsTest.cpp
	lib/samson/common/KVFileTest.cpp
	lib/samson/common/KVInputVectorTest.cpp
	lib/samson/common/LogsTest.cpp
	lib/samson/common/MemoryCheckTest.cpp

	# SAMSOM WORKER LIBRARY
	lib/samson/worker/SamsonWorkerTest.cpp

    #SAMSON SYSTEM LIBRARY
    lib/samson_system/ValueTest.cpp

	#SAMSON STREAM library
	lib/samson/stream/SharedMemoryManagerTest.cpp

	#SAMSON DELILAH LIBRARY
	lib/samson/delilah/common_delilah_test.h
	lib/samson/delilah/common_delilah_test.cpp
	lib/samson/delilah/delilahConsole_test.cpp	

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
     samson_system
     samsonModule
     samsonWorkerLib
     samsonStream
     samsonNetwork
     samsonCommon
     zookeeper_mt
     samson_comscore    
     samsonClient
     au
     xmlmarkup
     xmlparser  
     curses
     dl
     zoo
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/extra")
include_directories("${PROJECT_SOURCE_DIR}/apps")
include_directories("${PROJECT_SOURCE_DIR}/extern")
include_directories("/usr/include")

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(unitTest ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(unitTest ${STATIC_LIBS} )

#INSTALL(TARGETS               unitTest
#        RUNTIME DESTINATION   bin)
