SET (SOURCES
    main_passiveLocationGenerator.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonClient
     samsonWorkerLib
     delilahLib
     engine
     samsonModule
     samsonStream
     samsonIsolated
     samsonNetwork
     samsonCommon
     au
     lm
au
     pa
     protobuf
     dl
     pthread
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(passiveLocationGenerator ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(passiveLocationGenerator ${STATIC_LIBS} )

INSTALL(TARGETS               passiveLocationGenerator
        RUNTIME DESTINATION   bin)

