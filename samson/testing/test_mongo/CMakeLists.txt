# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("/opt/mongodb/mongo-cxx-driver-v2.0")
#include_directories("/opt/mongodb/mongo-cxx-driver-v1.8")
include_directories("/usr/local/include/")
include_directories("/opt/local/include/")

#
# Link directories
#-----------------------------------------------------------------
link_directories("/opt/mongodb/mongo-cxx-driver-v2.0")
#link_directories("/opt/mongodb/mongo-cxx-driver-v1.8")
link_directories("/opt/local/lib")


#Boost library
find_package(Boost COMPONENTS system filesystem REQUIRED)



INSTALL(TARGETS ${MODULE_NAME} DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})


# Exec file
# -----------------------------------------------------------------
ADD_EXECUTABLE(test_mongo mtest.cpp)
TARGET_LINK_LIBRARIES(test_mongo pa lm -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt -lmongoclient)



# TEST
# -----------------------------------------------------------------
# ADD_TEST(test_mongo ${PROJECT_SOURCE_DIR}/scripts/test_mongo )
