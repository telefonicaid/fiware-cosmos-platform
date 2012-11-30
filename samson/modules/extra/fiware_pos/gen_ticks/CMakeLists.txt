SET (SOURCES
    main_gen_ticks.cpp
)

SET (HEADERS
)

if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (SOLARIS_LIBS "-lsocket -lnsl")
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")

# Executable declaration
# ------------------------------------------------------------
ADD_EXECUTABLE(gen_ticks ${SOURCES} ${HEADERS} )

INSTALL(TARGETS gen_ticks DESTINATION ${PROJECT_SOURCE_DIR})
