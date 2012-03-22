# Fetch a list of tests
MESSAGE ("Setting up the test harness")
# Get a list of all .test files under the test directory
FILE (GLOB_RECURSE test_files ${CMAKE_CURRENT_SOURCE_DIR}/test *.test)
FOREACH ( test_path ${test_files} )
    MESSAGE ("${test_path}")
    GET_FILENAME_COMPONENT ( test_file ${test_path} NAME_WE )
    ADD_TEST( ${test_file} ${PROJECT_SOURCE_DIR}/scripts/samsonTest ${test_path} )
    set_tests_properties( ${test_file} PROPERTIES 
        FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
        PASS_REGULAR_EXPRESSION "SUCCESS;Ok" )
ENDFOREACH (test_path)
