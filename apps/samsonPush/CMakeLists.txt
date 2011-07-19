SET (SOURCES
    main_samsonPush.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonClient
     samsonWorkerLib
     samsonControllerLib
     delilahLib
     engine
     samsonModule
     samsonDataLib
     samsonStream
     samsonIsolated
     samsonNetwork
     samsonCommon
     pugi
     au
     lm
     pa
     protobuf
     dl
     pthread
     readline
     curses
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonPush ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonPush ${STATIC_LIBS})

INSTALL(TARGETS               samsonPush
        RUNTIME DESTINATION   bin)

