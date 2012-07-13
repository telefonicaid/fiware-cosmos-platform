SET (SOURCES
    main_SamsonCat.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonNetwork
     engine
     samsonModule
     delilahLib
     samsonCommon
     protobuf
     pthread
     au
     dl
     lm
	au
	 pa
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonCat ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonCat ${STATIC_LIBS})

INSTALL(TARGETS               samsonCat
        RUNTIME DESTINATION   bin)

