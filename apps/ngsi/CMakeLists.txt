SET (SOURCES
    main_ngsi.cpp
	Entity.cpp
	Verb.cpp
	jsonParse.cpp
	discoverContextAvailability.cpp
	registerContext.cpp
	ws.cpp
	httpData.cpp
	rest.cpp
	Peer.cpp
	Parameter.cpp
	duration.cpp
	logRequest.cpp
	versionRequest.cpp
	Registration.cpp
	Attribute.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     lm
     pa
	 json
	 xml2
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("/usr/local/include/json")
include_directories("/usr/include/libxml2")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(ngsi ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(ngsi ${STATIC_LIBS})

INSTALL(TARGETS               ngsi
        RUNTIME DESTINATION   bin)

