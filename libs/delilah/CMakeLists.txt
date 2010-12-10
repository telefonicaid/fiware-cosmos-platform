
SET (SOURCES
	Delilah.cpp
	DelilahConsole.cpp
	DelilahComponent.cpp
	DelilahDownloadDataProcess.cpp
	DelilahUploadDataProcess.cpp
)
	
SET (HEADERS
	Delilah.h
	DelilahConsole.h
	DelilahClient.h
	DelilahComponent.h
	DelilahDownloadDataProcess.h
	DelilahUploadDataProcess.h
)
	
SET (STATIC_LIBS
	samsonNetworkStatic
	samsonModuleStatic
	samsonCommonStatic
	samsonCodingStatic
	samsonMemoryStatic
	samsonNetworkStatic
	samsonDiskStatic
	samsonDataStatic
	protobuf
	dl
  	pthread
	ncurses
)	

	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/objects")
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/items")
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/gui")
	include_directories("${PROJECT_SOURCE_DIR}/libs/core")
	include_directories("${PROJECT_SOURCE_DIR}/libs/network")
	include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
	include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
	include_directories("${PROJECT_BINARY_DIR}/libs/network")
	include_directories("${PROJECT_SOURCE_DIR}/libs/common")
	include_directories("${PROJECT_SOURCE_DIR}/libs/module")
	include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
	include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
	include_directories("${PROJECT_SOURCE_DIR}/libs/data")
	include_directories("${PROJECT_SOURCE_DIR}/libs/module")

	ADD_LIBRARY(delilahStatic ${SOURCES} ${HEADERS})
	TARGET_LINK_LIBRARIES(delilahStatic  ${STATIC_LIBS})
