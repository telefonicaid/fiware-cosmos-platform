SET (SOURCES
    main_samsonWorker.cpp
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
    ${COMMON_SAMSON_LIBS}
)

include_directories("${PROJECT_SOURCE_DIR}/libs")


# SAMSON Executable
ADD_EXECUTABLE(samsonWorker ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonWorker ${LIBS})

INSTALL(TARGETS               samsonWorker
        RUNTIME DESTINATION   bin)
