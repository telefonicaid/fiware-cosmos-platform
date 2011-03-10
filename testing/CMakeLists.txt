ADD_SUBDIRECTORY(module_test)   # test module
ADD_SUBDIRECTORY(test_memory)   # test_memory
ADD_SUBDIRECTORY(test_disk)	    # test_disk
ADD_SUBDIRECTORY(test_hostMgr)  # test_hostMgr


# Prepare the files for the test
# --------------------------------------------------
add_custom_target( test_prepare ALL )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD  COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )

# Add the tests
# --------------------------------------------------
ADD_TEST(test_complete ${PROJECT_SOURCE_DIR}/scripts/test_complete >${CMAKE_BINARY_DIR}/testing/test_complete_stdout  2>${CMAKE_BINARY_DIR}/testing/test_complete_sterr)
