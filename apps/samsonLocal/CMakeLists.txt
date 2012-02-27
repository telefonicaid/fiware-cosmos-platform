SET (SOURCES
	FakeEndpoint.cpp
	NetworkFake.cpp
	NetworkFakeCenterPacket.cpp
	NetworkCenter.cpp
	main_samsonLocal.cpp
)

SET (HEADERS
	FakeEndpoint.h
	NetworkFake.h
	NetworkCenter.h
	NetworkFakeCenterPacket.h
)

SET (HEADERS_FOR_MOC
	DelilahQt.h
)

FIND_PACKAGE(Qt4 REQUIRED)

SET( STATIC_LIBS
    samsonWorkerLib
    delilahLib
    delilahQtLib
    engine
	samsonModule
	samsonStream
	samsonIsolated
	samsonNetwork
	samsonCommon
	au
	tables
	lm
	iconv
	pa
	protobuf
	dl
	pthread
	QtCore
	${COMMON_SAMSON_LIBS}
)


# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})		# Required for QT intermediate files to compile

# QT Operations
# ------------------------------------------------------------
#QT4_WRAP_CPP( HEADERS_MOC ${HEADERS_FOR_MOC} )
#QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
#QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})
# ------------------------------------------------------------

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonLocal ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonLocal ${STATIC_LIBS})
TARGET_LINK_LIBRARIES( samsonLocal ${QT_LIBRARIES} )

INSTALL(TARGETS               samsonLocal
        RUNTIME DESTINATION   bin)


