SET (SOURCES
    main_Delilah.cpp
)

SET (HEADERS
)

if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (SOLARIS_LIBS "-lsocket -lnsl")
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
SET (STATIC_LIBS
  	 zookeeper_mt
     delilahLib
     pa
     lm
     protobuf
     pthread
     curses
     samsonNetwork
     samsonStream
     samsonModule
     dl
     engine
     samsonCommon	
	 samsonZoo
	 samson_system
     au
     ${SOLARIS_LIBS}
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------
ADD_EXECUTABLE(delilah ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(delilah ${STATIC_LIBS} )

INSTALL(TARGETS               delilah
        RUNTIME DESTINATION   bin)
