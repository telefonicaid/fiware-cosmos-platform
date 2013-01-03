SET (SOURCES
    main_samsonWorker.cpp
)

SET (HEADERS
)

# SAMSON-956 - iconv functions are built into glibc on Linux but require a 
# separate library on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (ICONV_LIB "iconv")
else (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set (ICONV_LIB "")
    endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (SOLARIS_LIBS "-lsocket -lnsl -ljson-c")
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
SET (LIBS
    samsonWorkerLib
    delilahLib
    samsonNetwork
    samsonModule
    samsonStream
    samsonCommon
    zookeeper_mt
    samson_system
    engine
	zoo
    au
    ${ICONV_LIB}
    ${SOLARIS_LIBS}
    pa
    lm
    au
    protobuf
    pthread
    dl
    ${COMMON_SAMSON_LIBS}
)

include_directories("${PROJECT_SOURCE_DIR}/libs")


# SAMSON Executable
ADD_EXECUTABLE(samsonWorker ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonWorker ${LIBS})
INSTALL(TARGETS               samsonWorker        RUNTIME DESTINATION   bin)

ADD_EXECUTABLE(samsonResetZookeeper main_samsonResetZookeeper.cpp ${HEADERS})
TARGET_LINK_LIBRARIES(samsonResetZookeeper ${LIBS})
INSTALL(TARGETS               samsonResetZookeeper        RUNTIME DESTINATION   bin)
