# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")



# Exec file
# -----------------------------------------------------------------
ADD_EXECUTABLE(test_hostMgr main.cpp )
TARGET_LINK_LIBRARIES(test_hostMgr samsonNetwork protobuf lm pa pthread ${COMMON_SAMSON_LIBS})


# TEST
# -----------------------------------------------------------------
ADD_TEST(test_hostMgr ${PROJECT_SOURCE_DIR}/scripts/test_hostMgr )
