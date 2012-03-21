# Fetch a list of tests
FILE (GLOB test_0_files *.test)
FOREACH ( test_path ${test_0_files} )
    GET_FILENAME_COMPONENT ( test_file ${test_path} NAME_WE )
    ADD_TEST( ${test_file} ${PROJECT_SOURCE_DIR}/scripts/samsonDelilahTest ${test_path} )
    set_tests_properties( ${test_file} PROPERTIES 
        FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
        PASS_REGULAR_EXPRESSION "SUCCESS;Ok" )
ENDFOREACH (test_path)
