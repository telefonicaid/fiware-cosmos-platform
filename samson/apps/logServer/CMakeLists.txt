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

ADD_EXECUTABLE(logClient main_logClient.cpp  )
TARGET_LINK_LIBRARIES(logClient ${STATIC_LIBS})

ADD_EXECUTABLE(logProbe main_logProbe.cpp  )
TARGET_LINK_LIBRARIES(logProbe ${STATIC_LIBS})

ADD_EXECUTABLE(logQuery main_logQuery.cpp  )
TARGET_LINK_LIBRARIES(logQuery ${STATIC_LIBS})

ADD_EXECUTABLE(logCat main_logCat.cpp  )
TARGET_LINK_LIBRARIES(logCat ${STATIC_LIBS})

INSTALL(TARGETS logServer RUNTIME DESTINATION   bin)
INSTALL(TARGETS logClient RUNTIME DESTINATION   bin)
INSTALL(TARGETS logProbe  RUNTIME DESTINATION   bin)
INSTALL(TARGETS logQuery  RUNTIME DESTINATION   bin)
INSTALL(TARGETS logCat    RUNTIME DESTINATION   bin)

