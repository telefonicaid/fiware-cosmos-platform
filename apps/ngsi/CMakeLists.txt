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
	database.cpp
)

SET (HEADERS
)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
SET( STATIC_LIBS
     lm
	au
     pa
	 json
	 xml2
	 mysqlclient
	 z
	 m
	 cryptopp
)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF (${CMAKE_SYSTEM_TYPE} MATCHES "Ubuntu")
SET( STATIC_LIBS
     lm
     pa
	 json
	 xml2
	 mysqlclient
	 z
	 m
	 ssl3
	 nsl
	 cryptopp
)
ENDIF (${CMAKE_SYSTEM_TYPE} MATCHES "Ubuntu")

IF (${CMAKE_SYSTEM_TYPE} MATCHES "RedHat")
SET( STATIC_LIBS
     lm
     pa
	 json
	 xml2
	 mysqlclient
	 z
	 m
	 ssl3
	 nsl
	 crypto++
)
ENDIF (${CMAKE_SYSTEM_TYPE} MATCHES "RedHat")

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("/usr/local/include/json")
include_directories("/usr/include/json")
include_directories("/usr/include/libxml2")
include_directories("/usr/include/mysql")
include_directories("/opt/local/include/json")
include_directories("/usr/local/mysql-5.5.17-osx10.6-x86_64/include")

link_directories("/usr/local/mysql-5.5.17-osx10.6-x86_64/lib")

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(ngsi ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(ngsi ${STATIC_LIBS})

INSTALL(TARGETS               ngsi
        RUNTIME DESTINATION   bin)

