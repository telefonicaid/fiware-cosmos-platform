SET (HEADERS
    DataManager.h
    data.pb.h
)

SET (SOURCES
    DataManager.cpp
    data.pb.cc
)

SET (STATIC_LIBS
    samsonNetworkStatic 
    samsonModuleStatic 
    samsonCommonStatic 
    samsonCodingStatic
    samsonMemoryStatic
    samsonNetworkStatic 
    samsonDiskStatic
    protobuf 
    dl 
    pthread
    ncurses
)

include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

# Generate 'data.pb.h' and 'data.pb.cc' from ''data.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/data.pb.cc
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/data.pb.h
  COMMAND /usr/local/bin/protoc ${CMAKE_CURRENT_SOURCE_DIR}/data.proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY data.proto
)

ADD_LIBRARY(samsonDataStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonDataStatic ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonDataStatic PROPERTIES OUTPUT_NAME samsonData)


ADD_EXECUTABLE(samsonData main_samsonData.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonData ${STATIC_LIBS} )

INSTALL(TARGETS               samsonData
        RUNTIME DESTINATION   bin)
