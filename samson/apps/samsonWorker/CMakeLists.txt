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

SET (LIBS
    samsonWorkerLib
	delilahLib
    samsonNetwork
    samsonModule
    samsonStream
    samsonIsolated
    samsonCommon
	samson_system
    engine
    au
    ${ICONV_LIB}
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

INSTALL(TARGETS               samsonWorker
        RUNTIME DESTINATION   bin)
