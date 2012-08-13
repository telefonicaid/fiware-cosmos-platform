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

#SET (HEADERS_FOR_MOC
#    DelilahQt.h
#)

FIND_PACKAGE(Qt4 REQUIRED)

# SAMSON-956 - iconv functions are built into glibc on Linux but require a 
# separate library on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (ICONV_LIB "iconv")
else (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set (ICONV_LIB "")
    endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


SET( STATIC_LIBS
    samsonWorkerLib
    delilahLib
    #delilahQtLib
    engine
    samsonModule
    samsonStream
    samsonIsolated
    samsonNetwork
    samsonCommon
	samson_system
    au
    lm
	au
    ${ICONV_LIB}
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
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})        # Required for QT intermediate files to compile

# QT Operations
# ------------------------------------------------------------
#QT4_WRAP_CPP( HEADERS_MOC ${HEADERS_FOR_MOC} )
#QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
#QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})
# ------------------------------------------------------------

#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonLocal ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonLocal ${STATIC_LIBS})
#TARGET_LINK_LIBRARIES( samsonLocal ${QT_LIBRARIES} )

INSTALL(TARGETS               samsonLocal
        RUNTIME DESTINATION   bin)


