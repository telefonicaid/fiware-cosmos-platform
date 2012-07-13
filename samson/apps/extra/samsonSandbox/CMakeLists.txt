SET (SOURCES
	main_samsonSandbox.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
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

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonSandbox ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonSandbox ${STATIC_LIBS})

INSTALL(TARGETS               samsonSandbox
        RUNTIME DESTINATION   bin)

