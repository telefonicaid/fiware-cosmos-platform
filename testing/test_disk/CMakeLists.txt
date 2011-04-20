# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")



# Exec file
# -----------------------------------------------------------------
ADD_EXECUTABLE(test_disk main_test_disk.cpp )
TARGET_LINK_LIBRARIES(test_disk  engine samsonCommon  protobuf lm pa pthread ${COMMON_SAMSON_LIBS})


# TEST
# -----------------------------------------------------------------


ADD_TEST( test_disk ${PROJECT_SOURCE_DIR}/scripts/test_disk )
