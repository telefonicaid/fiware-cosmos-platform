SET (SOURCES
    main_SamsonConfig.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonNetwork
     engine
     samsonModule
     samsonCommon
     protobuf
     pthread
     au
     dl
     lm
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonConfig ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonConfig ${STATIC_LIBS})

INSTALL(TARGETS               samsonConfig
        RUNTIME DESTINATION   bin)

