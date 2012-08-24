if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (SOLARIS_LIBS "-lsocket -lnsl")
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")

SET( STATIC_LIBS
	 pa
     au
     lm
au
	 protobuf
     pthread
     ${SOLARIS_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(logServer main_logServer.cpp )
TARGET_LINK_LIBRARIES(logServer ${STATIC_LIBS})

INSTALL(TARGETS               logServer
        RUNTIME DESTINATION   bin)

ADD_EXECUTABLE(logClient main_logClient.cpp  )
TARGET_LINK_LIBRARIES(logClient ${STATIC_LIBS})

INSTALL(TARGETS               logClient
        RUNTIME DESTINATION   bin)

ADD_EXECUTABLE(logCat main_logCat.cpp  )
TARGET_LINK_LIBRARIES(logCat ${STATIC_LIBS})

INSTALL(TARGETS               logCat
        RUNTIME DESTINATION   bin)

