SET (SOURCES
    main_samsonPop.cpp
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
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonPop ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonPop ${STATIC_LIBS})

INSTALL(TARGETS               samsonPop
        RUNTIME DESTINATION   bin)

