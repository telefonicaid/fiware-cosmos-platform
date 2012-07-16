
# Prepare the files for the test
# --------------------------------------------------
add_custom_target( test_prepare ALL )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -rf ../../../../etc ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/testing/modules )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -rf ${PROJECT_BINARY_DIR}/libtienda.so ${PROJECT_BINARY_DIR}/../../system/build/libsystem.so ${PROJECT_BINARY_DIR}/testing/modules )

# Add the tests
# --------------------------------------------------
ADD_TEST(test_complete ${PROJECT_SOURCE_DIR}/testing/test_complete >${CMAKE_BINARY_DIR}/testing/test_complete_stdout  2>${CMAKE_BINARY_DIR}/testing/test_complete_sterr  )
