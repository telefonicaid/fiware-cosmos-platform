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

#include "logMsg/logMsg.h"

#include "common_delilah_test.h"


//Test void samson_file();
TEST(SamsonFileTest, samson_file)
{

   // Check error is detected in non-exiting files
   // ------------------------------------------------------------

    samson::SamsonFile samon_file_nonexisting( "/tmp/non_existing_file" );

    EXPECT_EQ(samon_file_nonexisting.hasError(), true) << "Wrong non existing /tmp/non_existing_file";
    EXPECT_EQ(samon_file_nonexisting.getErrorMessage(), "Not possible to open file") << "Wrong error message for /tmp/non_existing_file";

    samson::SamsonFile samon_file_nonsamson( "/bin/bash" );

    EXPECT_EQ(samon_file_nonsamson.hasError(), true) << "Wrong no samson samson_file detection for /bin/bash";
    EXPECT_EQ(samon_file_nonsamson.getErrorMessage(), "Wrong magic number in header") << "Wrong error message for /bin/bash";

	// delilah connections for tests
	// ------------------------------------------------------------
    samson::DelilahConsole *delilah_console = init_delilah_test();
	
    EXPECT_TRUE(delilah_console != NULL);

    char *host = strdup("localhost");
    int port = SAMSON_WORKER_PORT;
    char *env_port = getenv("SAMSON_WORKER_PORT_ENV");
    if (env_port != NULL)
    {
        port = atoi(env_port);
    }
    char *user = strdup("anonymous");
    char *password = strdup("anonymous");
    LM_M(("delilah_console->connect"));
    delilah_console->connect( host , port , user , password );

    delilah_console->runAsyncCommand("remove_all_stream");

    // Check with a txt file
    LM_M(("Pushing /tmp/words_input.txt to t queue"));
    delilah_console->runAsyncCommand("push /tmp/words_input.txt t");
    delilah_console->runAsyncCommand("pop t /tmp/output_file -force");
    LM_M(("Popped /tmp/output_file from t queue"));
    samson::SamsonFile samson_txt_dir( "/tmp/output_file" );
    EXPECT_EQ(samson_txt_dir.hasError(), true) << "Wrong ok samson_file detection for /tmp/output_file";
    EXPECT_EQ(samson_txt_dir.getErrorMessage(), "Getting header: read only -1 bytes (wanted to read 240)\n") << "Wrong error message for /tmp/output_file";
    EXPECT_EQ(samson_txt_dir.printContent(10, std::cout), 0) << "Wrong printContent for /tmp/output_file";
    EXPECT_EQ(strncmp(samson_txt_dir.getHashGroups().c_str(), "Error getting vector for hashgroups", strlen("Error getting vector for hashgroups")),0) << "Wrong getHashGroups for /tmp/output_file";

    samson::SamsonFile samson_txt_file( "/tmp/output_file/worker_000000_file_000001" );
    EXPECT_EQ(samson_txt_file.hasError(), true) << "Wrong ok samson_file detection for /tmp/output_file";
    EXPECT_EQ(samson_txt_file.getErrorMessage(), "Not possible to open file") << "Wrong error message for /tmp/output_file";
    EXPECT_EQ(samson_txt_file.printContent(10, std::cout), 0) << "Wrong printContent for /tmp/output_file";
    EXPECT_NE(strncmp(samson_txt_file.getHashGroups().c_str(), "Error getting vector for hashgroups", strlen("Error getting vector for hashgroups")),0) << "Wrong getHashGroups for /tmp/output_file";



    delilah_console->runAsyncCommand("push /bin/bash a");
    delilah_console->runAsyncCommand("pop a /tmp/test_SamsonFile -force");

	
	// Check with a simple file
	// ------------------------------------------------------------
    samson::SamsonFile samson_file( "/tmp/test_SamsonFile" );
    EXPECT_EQ(samson_file.hasError(), true) << "Wrong ok samson_file detection for /tmp/test_SamsonFile";
    EXPECT_EQ(samson_file.getErrorMessage(), "Getting header: read only -1 bytes (wanted to read 240)\n") << "Wrong error message for /tmp/test_SamsonFile";
    EXPECT_EQ(samson_file.printContent(10, std::cout), 0) << "Wrong printContent for /tmp/test_SamsonFile";
    EXPECT_EQ(strncmp(samson_file.getHashGroups().c_str(), "Error getting vector for hashgroups", strlen("Error getting vector for hashgroups")),0) << "Wrong getHashGroups for /tmp/test_SamsonFile";
	

	
	// Check with a binary file
	// ------------------------------------------------------------
	delilah_console->runAsyncCommand("remove_all_stream");
	
//    if (system ("word_generator 1000 | samsonPush input") == -1)
//	   std::cerr << "Error on word_generator system() command" << std::endl;

    LM_W(("system(push) sent successfully"));

    delilah_console->runAsyncCommandAndWait("push /tmp/words_input.txt input");

    delilah_console->runAsyncCommandAndWait("flush_buffers");
    delilah_console->runAsyncCommandAndWait("run txt.parser_words input words");
    delilah_console->runAsyncCommandAndWait("flush_buffers"); 
    delilah_console->runAsyncCommandAndWait("run txt.word_count_accumulate words words_count words_count");
    delilah_console->runAsyncCommandAndWait("flush_buffers"); 

    LM_W(("ready to pop words_count"));

    delilah_console->runAsyncCommandAndWait("pop words_count /tmp/test_SamsonFile_words -force");

    LM_W(("words_count popped"));
	
    samson::SamsonFile samson_file_ok( "/tmp/test_SamsonFile_words/worker_000000_file_000001" );
    EXPECT_EQ(samson_file_ok.hasError(), false) << "Wrong ok samson_file detection for /tmp/test_SamsonFile_words";
    EXPECT_EQ(samson_file_ok.getErrorMessage(), "No errors") << "Wrong error message for /tmp/test_SamsonFile_words";
    EXPECT_EQ(samson_file_ok.printContent(10, std::cerr), 10) << "Wrong printContent for /tmp/test_SamsonFile_words";
    EXPECT_EQ(strncmp(samson_file.getHashGroups().c_str(), "Error getting vector for hashgroups", strlen("Error getting vector for hashgroups")),0) << "Wrong getHashGroups for /tmp/test_SamsonFile_words";

     //Delilah disconnection
     //------------------------------------------------------------
    EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";
	
//    au::ErrorManager error;
//    delilah_console->delilah_disconnect( &error );
//    EXPECT_TRUE(error.isActivated() == false);
//    EXPECT_EQ(delilah_console->getPrompt(), "[Unconnected] Delilah>") << "Wrong prompt after disconnecting";

    close_delilah_test(delilah_console);
}
