
# Prepare the files for the test
# --------------------------------------------------
add_custom_target( test_prepare ALL )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_1_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_1_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_2_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_2_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_3_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_3_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_4_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_4_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_5_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_5_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -rf ../../../../etc ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/testing/modules )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp  ${PROJECT_BINARY_DIR}/liburl.so ${PROJECT_BINARY_DIR}/../../system/build/libsystem.so ${PROJECT_BINARY_DIR}/testing/modules )

# Add the tests
# --------------------------------------------------
ADD_TEST(test_1_complete ${PROJECT_SOURCE_DIR}/testing/test_1_complete >${CMAKE_BINARY_DIR}/testing/test_1_complete_stdout  2>${CMAKE_BINARY_DIR}/testing/test_1_complete_sterr  )
ADD_TEST(test_2_complete ${PROJECT_SOURCE_DIR}/testing/test_2_complete >${CMAKE_BINARY_DIR}/testing/test_2_complete_stdout  2>${CMAKE_BINARY_DIR}/testing/test_2_complete_sterr  )
ADD_TEST(test_3_complete ${PROJECT_SOURCE_DIR}/testing/test_3_complete >${CMAKE_BINARY_DIR}/testing/test_3_complete_stdout  2>${CMAKE_BINARY_DIR}/testing/test_3_complete_sterr  )
ADD_TEST(test_4_complete ${PROJECT_SOURCE_DIR}/testing/test_4_complete >${CMAKE_BINARY_DIR}/testing/test_4_complete_stdout  2>${CMAKE_BINARY_DIR}/testing/test_4_complete_sterr  )
ADD_TEST(test_5_complete ${PROJECT_SOURCE_DIR}/testing/test_5_complete >${CMAKE_BINARY_DIR}/testing/test_5_complete_stdout  2>${CMAKE_BINARY_DIR}/testing/test_5_complete_sterr  )
