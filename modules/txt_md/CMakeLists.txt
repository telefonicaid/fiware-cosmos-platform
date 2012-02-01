SET( MODULE_NAME txt_md )

cmake_minimum_required(VERSION 2.6)

# Locate samsonModuleParser
if (SAMSONMODULEPARSER)
    MESSAGE ("Using ${SAMSONMODULEPARSER} to parse the module file")
else (NOT SAMSONMODULEPARSER)
    FIND_PROGRAM(HAVE_SAMSONMODULEPARSER samsonModuleParser HINTS ${PROJECT_BINARY_DIR}/apps/samsonModuleParser/samsonModuleParser ENV PATH)
    if (HAVE_SAMSONMODULEPARSER)
        SET (SAMSONMODULEPARSER ${HAVE_SAMSONMODULEPARSER} CACHE INTERNAL "")
    else (NOT HAVE_SAMSONMODULEPARSER)
        MESSAGE("*****************************")
        MESSAGE("")
        MESSAGE("samsonModuleParser not found")
        MESSAGE("")
        MESSAGE("*****************************")
    endif (HAVE_SAMSONMODULEPARSER)
endif (SAMSONMODULEPARSER)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
    COMMAND ${SAMSONMODULEPARSER} ${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME}/module ${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME} Module
)

SET (SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
)

# Take all .cpp file in the compilation
FILE( GLOB ADDITIONAL_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp  )

# Install headers to be used by other modules
FILE( GLOB HEADERS samson/modules/${MODULE_NAME}/*.h  )

# Include directory pointing to current dir and /usr/local/include
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("/usr/local/include/")
include_directories("${CMAKE_INSTALL_PREFIX}/include")
include_directories("/opt/local/include/")
include_directories("/opt/mongodb/mongo-cxx-driver-v2.0")
include_directories("/opt/hadoop/hadoop-0.21.0/hdfs/src/c++/libhdfs")
include_directories("/usr/lib/jvm/java-6-sun-1.6.0.26/include")
include_directories("/usr/lib/jvm/java-6-sun-1.6.0.26/include/linux")
include_directories("/usr/lib/jvm/java-1.6.0-sun-1.6.0.22.x86_64/include")
include_directories("/usr/lib/jvm/java-1.6.0-sun-1.6.0.22.x86_64/include/linux")
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME}")
include_directories("${PROJECT_SOURCE_DIR}/modules/system")
include_directories("${PROJECT_BINARY_DIR}/modules/${MODULE_NAME}")



link_directories("/opt/hadoop/hadoop-0.21.0/c++/Linux-i386-32/lib")
link_directories("/opt/hadoop/hadoop-0.21.0/c++/Linux-amd64-64/lib")
link_directories("/usr/lib/jvm/java-6-sun-1.6.0.26/jre/lib/i386/client")
link_directories("/usr/lib/jvm/java-6-sun-1.6.0.26/jre/lib/amd64/server")
link_directories("/usr/lib/jvm/java-1.6.0-sun-1.6.0.22.x86_64/jre/lib/amd64/server")


link_directories("/opt/mongodb/mongo-cxx-driver-v2.0")
link_directories("/opt/local/lib")

MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")

find_package(Boost COMPONENTS system filesystem REQUIRED)

ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES} ${ADDITIONAL_SOURCES}  )
#target_link_libraries(${MODULE_NAME} libmongoclient.a -lboost_thread-mt -lboost_filesystem -lhdfs -ljvm)
target_link_libraries(${MODULE_NAME}  -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt -lmongoclient )

INSTALL(TARGETS ${MODULE_NAME} DESTINATION modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})

#ENABLE_TESTING()
#ADD_SUBDIRECTORY(testing)     # Test module and ctest stuff
