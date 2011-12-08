SET (SOURCES
    main_samsonDiskTest.cpp
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
ADD_EXECUTABLE(samsonDiskTest ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonDiskTest ${LIBS})

INSTALL(TARGETS               samsonDiskTest
        RUNTIME DESTINATION   bin)
