SET (SOURCES
    main_samsonController.cpp
)

SET (HEADERS

)

SET (LIBS
	samsonControllerLib
	samsonNetwork
	samsonModule
	engine
	au
	pa
	lm
	protobuf
	pthread
	dl
	readline
	samsonDataLib
	samsonCommon
	${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")



# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonController ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonController ${LIBS})


INSTALL(TARGETS               samsonController
        RUNTIME DESTINATION   bin)
