SET (HEADERS
    SimpleDataManager.h
    DataManager.h
    DataManagerItem.h
    DataManagerCommandResponse.h
    LogFile.h
    data.pb.h
)

SET (SOURCES
    SimpleDataManager.cpp
    DataManager.cpp
    DataManagerItem.cpp
    DataManagerCommandResponse.cpp
    LogFile.cpp
    data.pb.cc
)

# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

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


