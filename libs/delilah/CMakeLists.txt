
SET (SOURCES
	Delilah.cpp
	DelilahConsole.cpp
	DelilahComponent.cpp
	DelilahDownloadDataProcess.cpp
	DelilahUploadDataProcess.cpp
	TXTFileSet.cpp
)
	
SET (HEADERS
	Delilah.h
	DelilahConsole.h
	DelilahClient.h
	DelilahComponent.h
	DelilahDownloadDataProcess.h
	DelilahUploadDataProcess.h
	TXTFileSet.h
)
	
# Include directories
# -----------------------------------------------------------------
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

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(delilahLib ${SOURCES} ${HEADERS})
