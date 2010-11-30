SET (HEADERS
	BufferVector.h
	DeviceDiskAccessManager.h
	DeviceDiskAccessStatistics.h
	DiskManager.h
	DiskManagerDelegate.h
	DiskOperation.h
	DiskOperationsManager.h
	FileAccess.h
	FileManager.h
)

SET (SOURCES
	BufferVector.cpp
	DeviceDiskAccessManager.cpp
	DeviceDiskAccessStatistics.cpp
	DiskManager.cpp
	DiskOperation.cpp
	DiskOperationsManager.cpp
	FileAccess.cpp
	FileManager.cpp
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


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

ADD_LIBRARY(samsonDiskStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonDiskStatic  ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonDiskStatic PROPERTIES OUTPUT_NAME samsonDisk)

# ADD_LIBRARY(samsonDiskShared SHARED ${SOURCES} ${HEADERS} )
# TARGET_LINK_LIBRARIES(samsonDiskShared  ${LIBS_SHARED} )
# SET_TARGET_PROPERTIES(samsonDiskShared PROPERTIES OUTPUT_NAME samsonDisk)


