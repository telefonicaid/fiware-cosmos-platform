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
#include "samson/delilah/Delilah.h"                                     // samson::Delailh
#include "samson/delilah/DelilahConsole.h"
#include "common_delilah_test.h"


//Test void getInfo( std::ostringstream& output);
TEST(delilahConsoleTest, DISABLED_getPrompt)
{
    samson::DelilahConsole *delilah_console = init_delilah_test();

    EXPECT_TRUE(delilah_console != NULL);
    EXPECT_EQ(delilah_console->getPrompt(), "[Unconnected] Delilah>") << "Wrong prompt before connecting";
    // Stupid warning on deprecated conversion from string constant to ‘char*’
    char *host = strdup("localhost");
    int port = SAMSON_WORKER_PORT;
    char *user = strdup("anonymous");
    char *password = strdup("anonymous");
    delilah_console->connect( host , port , user , password );
    char expected_result[1024];
    sprintf(expected_result, "[%s@%s:%d] Delilah>", user, host, port);
    EXPECT_EQ(delilah_console->getPrompt(), expected_result) << "Wrong prompt after connected (samsonWorker should bu running in localhost)";

    au::ErrorManager error;
    delilah_console->delilah_disconnect( &error );
    EXPECT_TRUE(error.isActivated() == false);
    EXPECT_EQ(delilah_console->getPrompt(), "[Unconnected] Delilah>") << "Wrong prompt after disconnecting";


    //close_delilah_test(delilah_console);
}


