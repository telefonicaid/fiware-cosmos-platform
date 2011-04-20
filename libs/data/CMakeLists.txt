SET (HEADERS
    DataManager.h
    DataManagerItem.h
    DataManagerCommandResponse.h
    LogFile.h
    data.pb.h
)

SET (SOURCES
    DataManager.cpp
    DataManagerItem.cpp
    DataManagerCommandResponse.cpp
    LogFile.cpp
    data.pb.cc
)

# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")

# Generate 'data.pb.h' and 'data.pb.cc' from ''data.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/data.pb.cc
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/data.pb.h
  COMMAND protoc ${CMAKE_CURRENT_SOURCE_DIR}/data.proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY data.proto
)

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonDataLib STATIC ${SOURCES} ${HEADERS} )

