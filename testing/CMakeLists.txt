ADD_SUBDIRECTORY(module_test)	# test module


# Prepare the test
#SET( TEST_COMMAND ../apps/samsonLocal/samsonLocal -working . -f ${PROJECT_SOURCE_DIR}/testing/commands.txt > test_std_output 2> test_std_error )
#add_custom_target( test_prepare ALL touch test.log )
#add_custom_command ( TARGET test_prepare PRE_BUILD   COMMAND echo '***** Running complete test of SAMSON' )
#add_custom_command ( TARGET test_prepare PRE_BUILD   COMMAND mkdir modules 2>/dev/null  || true )
#add_custom_command ( TARGET test_prepare PRE_BUILD   COMMAND cp -f module_test/*.so modules  )
#add_custom_command ( TARGET test_prepare PRE_BUILD   COMMAND ${TEST_COMMAND} )
#add_custom_command ( TARGET test_prepare PRE_BUILD   COMMAND cat out.txt | sort -n > out2.txt )
#add_custom_command ( TARGET test_prepare POST_BUILD  COMMAND echo '***** Running complete test of SAMSON [FINISH]' )
#add_dependencies(test_prepare module_test)

# Add the test to check globally samsonLocal
#ADD_TEST(test_complete diff ${CMAKE_BINARY_DIR}/testing/out2.txt ${PROJECT_SOURCE_DIR}/testing/out.txt )


#Prepare the files for the test
add_custom_target( test_prepare ALL touch test.log )
add_custom_command ( TARGET test_prepare PRE_BUILD   COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_complete_commands.txt ${PROJECT_BINARY_DIR}/testing )
add_custom_command ( TARGET test_prepare PRE_BUILD   COMMAND cp -f ${PROJECT_SOURCE_DIR}/testing/test_complete_expected_out.txt ${PROJECT_BINARY_DIR}/testing )

# Add the tests
ADD_TEST(test_complete ${PROJECT_SOURCE_DIR}/scripts/test_complete >${CMAKE_BINARY_DIR}/out_test_complete  )
