SET (HEADERS
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
	DeviceDiskAccessManager.cpp
	DeviceDiskAccessStatistics.cpp
	DiskManager.cpp
	DiskOperation.cpp
	DiskOperationsManager.cpp
	FileAccess.cpp
	FileManager.cpp
)

SET( LIBS_STATIC
     logMsgStatic
     samsonCommonStatic
     protobuf
     ncurses
     pthread
)

SET( LIBS_SHARED
     samsonCommonShared
     protobuf
     ncurses
     pthread
)

include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

ADD_LIBRARY(samsonDiskStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonDiskStatic  ${LIBS_STATIC} )
SET_TARGET_PROPERTIES(samsonDiskStatic PROPERTIES OUTPUT_NAME samsonDisk)

# ADD_LIBRARY(samsonDiskShared SHARED ${SOURCES} ${HEADERS} )
# TARGET_LINK_LIBRARIES(samsonDiskShared  ${LIBS_SHARED} )
# SET_TARGET_PROPERTIES(samsonDiskShared PROPERTIES OUTPUT_NAME samsonDisk)


ADD_EXECUTABLE(samsonFileManagerTest samsonFileManagerTest.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonFileManagerTest  ${LIBS_STATIC} )
INSTALL(TARGETS               samsonFileManagerTest
			      RUNTIME DESTINATION   bin)
