/* ****************************************************************************
*
* FILE            SamsonFile_test.cpp
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


//Test void samson_file();
TEST(SamsonFileTest, samson_file)
{

    samson::SamsonFile samon_file_nonexisting( "/tmp/non_existing_file" );

    EXPECT_EQ(samon_file_nonexisting.hasError(), true) << "Wrong non existing /tmp/non_existing_file";
    EXPECT_EQ(samon_file_nonexisting.getErrorMessage(), "Not possible to open file") << "Wrong error message for /tmp/non_existing_file";

    samson::SamsonFile samon_file_nonsamson( "/bin/bash" );

    EXPECT_EQ(samon_file_nonsamson.hasError(), true) << "Wrong no samson samson_file detection for /bin/bash";
    EXPECT_EQ(samon_file_nonsamson.getErrorMessage(), "Wrong magic number in header") << "Wrong error message for /bin/bash";

    samson::DelilahConsole *delilah_console = init_delilah_test();

    EXPECT_TRUE(delilah_console != NULL);

    char *host = strdup("localhost");
    int port = SAMSON_WORKER_PORT;
    char *user = strdup("anonymous");
    char *password = strdup("anonymous");
    delilah_console->connect( host , port , user , password );

    delilah_console->runAsyncCommand("remove_all_stream");

    delilah_console->runAsyncCommand("push /bin/bash a");
    delilah_console->runAsyncCommand("pop a /tmp/test_SamsonFile -force");

    samson::SamsonFile samson_file( "/tmp/test_SamsonFile" );
    EXPECT_EQ(samson_file.hasError(), true) << "Wrong ok samson_file detection for /tmp/test_SamsonFile";
    EXPECT_EQ(samson_file.getErrorMessage(), "Getting header: read only -1 bytes (wanted to read 240)\n") << "Wrong error message for /tmp/test_SamsonFile";
    EXPECT_EQ(samson_file.printContent(10, std::cout), 0) << "Wrong printContent for /tmp/test_SamsonFile";
    EXPECT_EQ(strncmp(samson_file.getHashGroups().c_str(), "Error getting vector for hashgroups", strlen("Error getting vector for hashgroups")),0) << "Wrong getHashGroups for /tmp/test_SamsonFile";

    if (system ("word_generator 1000 | samsonPush input") == -1)
    {
        std::cerr << "Error on word_generator system() command" << std::endl;
    }
    delilah_console->runAsyncCommand("run txt.parser_words input words");
    sleep(1);
    delilah_console->runAsyncCommand("run txt.word_count_accumulate words words_count words_count");
    sleep(1);
    delilah_console->runAsyncCommand("pop words_count /tmp/test_SamsonFile_words -force");
    sleep(1);

    samson::SamsonFile samson_file_ok( "/tmp/test_SamsonFile_words/worker_000000_file_000001" );
    EXPECT_EQ(samson_file_ok.hasError(), false) << "Wrong ok samson_file detection for /tmp/test_SamsonFile_words";
    EXPECT_EQ(samson_file_ok.getErrorMessage(), "No errors") << "Wrong error message for /tmp/test_SamsonFile_words";
    EXPECT_EQ(samson_file_ok.printContent(10, std::cout), 10) << "Wrong printContent for /tmp/test_SamsonFile_words";
    EXPECT_EQ(strncmp(samson_file.getHashGroups().c_str(), "Error getting vector for hashgroups", strlen("Error getting vector for hashgroups")),0) << "Wrong getHashGroups for /tmp/test_SamsonFile_words";


    au::ErrorManager error;
    delilah_console->delilah_disconnect( &error );
    //EXPECT_TRUE(error.isActivated() == false);

    //close_delilah_test(delilah_console);
}
