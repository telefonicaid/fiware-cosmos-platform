
# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")


# Test file
# -----------------------------------------------------------------
ADD_EXECUTABLE(test_memory main_memory_test.cpp )
TARGET_LINK_LIBRARIES(test_memory engine samsonCommon  protobuf lm pthread ${COMMON_SAMSON_LIBS})
ADD_TEST(test_memory ${PROJECT_BINARY_DIR}/testing/test_memory/test_memory  )
