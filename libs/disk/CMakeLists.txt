SET (HEADERS
	BufferVector.h
	DeviceDiskAccessManager.h
	DeviceDiskAccessStatistics.h
	DiskManager.h
	DiskManagerDelegate.h
	DiskOperation.h
	FileManager.h
	FileManagerItem.h
	FileManagerReadItem.h
	FileManagerWriteItem.h
	FileManagerCacheItem.h
	FileManagerCacheTask.h
	FileManagerCacheSystem.h
	DiskStatistics.h
)

SET (SOURCES
	BufferVector.cpp
	DeviceDiskAccessManager.cpp
	DeviceDiskAccessStatistics.cpp
	DiskManager.cpp
	DiskOperation.cpp
	FileManager.cpp
	FileManagerItem.cpp
	FileManagerReadItem.cpp
	FileManagerWriteItem.cpp
    FileManagerCacheItem.cpp
    FileManagerCacheTask.cpp
    FileManagerCacheSystem.cpp
	DiskStatistics.cpp
)

# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonDisk STATIC ${SOURCES} ${HEADERS} )



