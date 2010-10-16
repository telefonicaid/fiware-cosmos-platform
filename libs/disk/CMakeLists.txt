SET (HEADERS

	DeviceDiskAccessManager.h
	DeviceDiskAccessStatistics.h
	DiskManager.h
	DiskManagerDelegate.h
	DiskOperation.h
	DiskOperationsManager.h
	FileAccess.h
)

SET (SOURCES
	DeviceDiskAccessManager.cpp
	DeviceDiskAccessStatistics.cpp
	DiskManager.cpp
	DiskOperation.cpp
	DiskOperationsManager.cpp
	FileAccess.cpp
)

SET( LIBS_STATIC
     samsonCommonStatic
     protobuf
     ncurses
)

SET( LIBS_SHARED
     samsonCommonShared
     protobuf
     ncurses
)

include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")


ADD_LIBRARY(samsonDiskStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonDiskStatic  ${LIBS_STATIC} )
SET_TARGET_PROPERTIES(samsonDiskStatic PROPERTIES OUTPUT_NAME samsonDisk)

ADD_LIBRARY(samsonDiskShared SHARED ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonDiskShared  ${LIBS_SHARED} )
SET_TARGET_PROPERTIES(samsonDiskShared PROPERTIES OUTPUT_NAME samsonDisk)



