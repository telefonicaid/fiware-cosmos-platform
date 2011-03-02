ADD_SUBDIRECTORY(module_test)	# test module
ADD_SUBDIRECTORY(test_memory)	# test_memory


# Prepare the files for the test
# --------------------------------------------------
add_custom_target( test_prepare ALL touch test.log )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )

# Add the tests
# --------------------------------------------------
ADD_TEST(test_complete ${PROJECT_SOURCE_DIR}/scripts/test_complete >${CMAKE_BINARY_DIR}/out_test_complete  )
