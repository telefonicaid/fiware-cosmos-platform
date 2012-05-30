/* ****************************************************************************
*
* FILE            delilah_test.cpp
*
* AUTHOR         Gregorio Escalada
*
* DATE            May 2012
*
* DESCRIPTION
*
* unit testing of the delilah class in the samson  library
*
*/

#include "gtest/gtest.h"

#include "samson/common/ports.h" // for SAMSON_WORKER_PORT
#include "samson/delilah/SamsonFile.h"                                     // samson::Delailh

#include "common_delilah_test.h"


//Test void getPrompt();
TEST(SamsonFileTest, DISABLED_getPrompt)
{
    samson::SamsonFile samon_file_nonexisting( "/tmp/non_existing_file" );

    EXPECT_EQ(samon_file_nonexisting.hasError(), true) << "Wrong non existing samson_file";
    EXPECT_EQ(samon_file_nonexisting.getErrorMessage(), "Not possible to open file") << "Wrong error message";

    samson::SamsonFile samon_file_nonsamson( "/bin/bash" );

    EXPECT_EQ(samon_file_nonsamson.hasError(), true) << "Wrong no samson samson_file detection";
    EXPECT_EQ(samon_file_nonsamson.getErrorMessage(), "Wrong magic number in header") << "Wrong error message";

    samson::DelilahConsole *delilah_console = init_delilah_test();

    EXPECT_TRUE(delilah_console != NULL);

    char *host = strdup("localhost");
    int port = SAMSON_WORKER_PORT;
    char *user = strdup("anonymous");
    char *password = strdup("anonymous");
    delilah_console->connect( host , port , user , password );

    delilah_console->runAsyncCommand("push /bin/bash a");
    delilah_console->runAsyncCommand("pop a /tmp/test_SamsonFile");

    samson::SamsonFile samson_file( "/bin/test_SamsonFile" );
    EXPECT_EQ(samson_file.hasError(), true) << "Wrong ok samson_file detection";
    EXPECT_EQ(samson_file.getErrorMessage(), "No errors") << "Wrong error message";
    EXPECT_EQ(samson_file.printContent(10, std::cout), 10) << "Wrong printContent";
    EXPECT_EQ(samson_file.getHashGroups(), "hashgroups") << "Wrong getHashGroups";

    au::ErrorManager error;
    delilah_console->delilah_disconnect( &error );
    //EXPECT_TRUE(error.isActivated() == false);

    close_delilah_test(delilah_console);
}
