# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("/opt/mongodb/mongo-cxx-driver-v2.0")


#
# Link directories
#-----------------------------------------------------------------
link_directories("/opt/mongodb/mongo-cxx-driver-v2.0")


# Exec file
# -----------------------------------------------------------------
ADD_EXECUTABLE(test_mongo mtest.cpp)
TARGET_LINK_LIBRARIES(test_mongo pa lm mongoclient -lboost_thread-mt -lboost_filesystem)



# TEST
# -----------------------------------------------------------------
# ADD_TEST(test_mongo ${PROJECT_SOURCE_DIR}/scripts/test_mongo )
