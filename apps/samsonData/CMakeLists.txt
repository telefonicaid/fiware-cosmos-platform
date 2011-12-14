SET (SOURCES
    main_samsonData.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonNetwork
     engine
     samsonCommon
     samsonDataLib
     delilahLib
     protobuf
     pthread
     au
     dl
     lm
     pa
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonData ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonData ${STATIC_LIBS})

INSTALL(TARGETS               samsonData
        RUNTIME DESTINATION   bin)

