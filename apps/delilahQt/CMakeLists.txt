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
     ${COMMON_SAMSON_LIBS}
)


include_directories("${PROJECT_SOURCE_DIR}/libs")

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

