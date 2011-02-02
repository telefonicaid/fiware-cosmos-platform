SET (SOURCES
    main_DelilahQT.cpp
)

SET (HEADERS
)

SET (STATIC_LIBS
     samsonNetworkStatic
     samsonCoreStatic
     samsonCommonStatic
     delilahStatic
     pa
     lm
     protobuf
     pthread
     dl
     curses
)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/delilah")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")

SET (RESOURCES
    ${PROJECT_SOURCE_DIR}/libs/delilah/resources/objects.qrc
    ${PROJECT_SOURCE_DIR}/libs/delilah/resources/icons.qrc)

QT4_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})

link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")
link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

# SAMSON Executable
ADD_EXECUTABLE(delilahQT ${SOURCES} ${HEADERS} ${RESOURCES_QRC} )
TARGET_LINK_LIBRARIES(delilahQT ${STATIC_LIBS} )

INSTALL(TARGETS               delilahQT
        RUNTIME DESTINATION   bin)

