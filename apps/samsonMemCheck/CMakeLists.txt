SET (SOURCES
    main_SamsonMemCheck.cpp
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
	au
	 pa
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")

# Executable declaration
# ------------------------------------------------------------

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    ADD_DEFINITIONS(-DOSX)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    ADD_DEFINITIONS(-DLINUX)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

ADD_EXECUTABLE(samsonMemCheck ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonMemCheck ${STATIC_LIBS})

INSTALL(TARGETS               samsonMemCheck
        RUNTIME DESTINATION   bin)

