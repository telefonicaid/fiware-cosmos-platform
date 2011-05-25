SET (SOURCES
    main_samsonWorker.cpp
)

SET (HEADERS
)


SET (LIBS
    samsonWorkerLib
    samsonNetwork
    samsonModule
    samsonCommon
    samsonStream
    engine
    au
    pa
    lm
    protobuf
    pthread
    dl
    readline
    ${COMMON_SAMSON_LIBS}
)

include_directories("${PROJECT_SOURCE_DIR}/libs")


# SAMSON Executable
ADD_EXECUTABLE(samsonWorker2 ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonWorker2 ${LIBS})

INSTALL(TARGETS               samsonWorker2
        RUNTIME DESTINATION   bin)
