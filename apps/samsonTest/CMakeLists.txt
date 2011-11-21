SET (SOURCES
    main_samsonTest.cpp
)

SET (HEADERS
)


SET (LIBS
    samsonWorkerLib
    samsonIsolated
    samsonNetwork
    samsonModule
    samsonStream
    samsonCommon
    samsonDataLib
    engine
    au
    pa
    lm
    protobuf
    pthread
    dl
    edit
    ${COMMON_SAMSON_LIBS}
)

include_directories("${PROJECT_SOURCE_DIR}/libs")


# SAMSON Executable
ADD_EXECUTABLE(samsonTest ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonTest ${LIBS})

INSTALL(TARGETS               samsonTest
        RUNTIME DESTINATION   bin)
