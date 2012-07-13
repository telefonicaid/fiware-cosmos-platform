SET( HEADERS
TEST_Module.h
)

SET( SOURCES
TEST_General.cpp
TEST_Vectores.cpp
TEST_Adjacent.cpp
TEST_MobVar.cpp
TEST_Itineraries.cpp
TEST_Matrix.cpp
TEST_ActivDensity.cpp
TEST_PopulDensity.cpp
TEST_Utils.cpp
TEST_Borrar.cpp
VoronoiDiagramGenerator.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/au")
include_directories("${PROJECT_SOURCE_DIR}/kv")
include_directories("${PROJECT_SOURCE_DIR}/macro_lib")
include_directories("${PROJECT_SOURCE_DIR}/macro_base")
include_directories("${PROJECT_SOURCE_DIR}/macro_gasset")

ADD_LIBRARY( macro_test ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( macro_test macro_lib macro_base macro_gasset kv au pthread)

# Install defined scripts
FILE(GLOB files "scripts/*.macro")
install(FILES ${files} DESTINATION share/macro/scripts)
