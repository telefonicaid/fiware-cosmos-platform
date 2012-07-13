
# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")

#add_definitions( -fprofile-arcs -ftest-coverage )


# Test file
# -----------------------------------------------------------------
ADD_EXECUTABLE(test_unit main_test_unit.cpp )
TARGET_LINK_LIBRARIES(test_unit  engine samsonCommon  protobuf lm pthread cppunit gcov)
ADD_TEST(test_unit ${PROJECT_BINARY_DIR}/testing/test_unit/test_unit  )
