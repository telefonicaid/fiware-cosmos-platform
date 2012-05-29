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


//Test void getPrompt();
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


    close_delilah_test(delilah_console);
}

//Test size_t runAsyncCommand( std::string command);
TEST(delilahConsoleTest, runAsyncCommand)
{
    samson::DelilahConsole *delilah_console = init_delilah_test();

    EXPECT_TRUE(delilah_console != NULL);

    char *host = strdup("localhost");
    int port = SAMSON_WORKER_PORT;
    char *user = strdup("anonymous");
    char *password = strdup("anonymous");
    delilah_console->connect( host , port , user , password );

    EXPECT_NE(delilah_console->runAsyncCommand("test_trap"), 0) << "Wrong result from runAsyncCommand(test_trap)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode database"), 0) << "Wrong result from runAsyncCommand(set_mode database)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode mode_logs"), 0) << "Wrong result from runAsyncCommand(set_mode mode_logs)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from runAsyncCommand(set_mode normal)";

    EXPECT_EQ(delilah_console->runAsyncCommand("history"), 0) << "Wrong result from runAsyncCommand(history)";

    EXPECT_EQ(delilah_console->runAsyncCommand("history 10"), 0) << "Wrong result from runAsyncCommand(history 10)";

    EXPECT_EQ(delilah_console->runAsyncCommand("cluster info"), 0) << "Wrong result from runAsyncCommand(cluster info)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help"), 0) << "Wrong result from runAsyncCommand(help)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help category"), 0) << "Wrong result from runAsyncCommand(help category)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help all"), 0) << "Wrong result from runAsyncCommand(help all)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help ls"), 0) << "Wrong result from runAsyncCommand(help ls)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help ls_stream_operation"), 0) << "Wrong result from runAsyncCommand(help ls_stream_operation)";

    EXPECT_EQ(delilah_console->runAsyncCommand("threads"), 0) << "Wrong result from runAsyncCommand(threads)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set"), 0) << "Wrong result from runAsyncCommand(set)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set a b"), 0) << "Wrong result from runAsyncCommand(set a b)";

    EXPECT_EQ(delilah_console->runAsyncCommand("unset a"), 0) << "Wrong result from runAsyncCommand(unset a)";

    EXPECT_EQ(delilah_console->runAsyncCommand("alerts on"), 0) << "Wrong result from runAsyncCommand(alerts on)";

    EXPECT_EQ(delilah_console->runAsyncCommand("alerts off"), 0) << "Wrong result from runAsyncCommand(alerts off)";

    EXPECT_EQ(delilah_console->runAsyncCommand("show_alerts"), 0) << "Wrong result from runAsyncCommand(show_alerts)";

    EXPECT_EQ(delilah_console->runAsyncCommand("verbose"), 0) << "Wrong result from runAsyncCommand(verbose)";

    EXPECT_EQ(delilah_console->runAsyncCommand("open_alerts_file /tmp/trace.log"), 0) << "Wrong result from runAsyncCommand(open_alerts_file /tmp/trace.log)";

    EXPECT_EQ(delilah_console->runAsyncCommand("close_alerts_file"), 0) << "Wrong result from runAsyncCommand(close_alerts_file)";

    EXPECT_EQ(delilah_console->runAsyncCommand("clear_components"), 0) << "Wrong result from runAsyncCommand(clear_components)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps"), 0) << "Wrong result from runAsyncCommand(ps)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps 1"), 0) << "Wrong result from runAsyncCommand(ps 1)";

    EXPECT_EQ(delilah_console->runAsyncCommand("push"), 0) << "Wrong result from runAsyncCommand(push)";

    EXPECT_NE(delilah_console->runAsyncCommand("push /bin/bash a"), 0) << "Wrong result from runAsyncCommand(push /bin/bash a)";

    EXPECT_NE(delilah_console->runAsyncCommand("push /opt/samson/bin/samsonWorker b"), 0) << "Wrong result from runAsyncCommand(push /opt/samson/bin/samsonWorker b)";

    EXPECT_NE(delilah_console->runAsyncCommand("pop a /tmp/traces"), 0) << "Wrong result from runAsyncCommand(pop a /tmp/traces)";

    EXPECT_NE(delilah_console->runAsyncCommand("pop a /tmp/traces2"), 0) << "Wrong result from runAsyncCommand(pop a /tmp/traces2)";

    EXPECT_EQ(delilah_console->runAsyncCommand("rm"), 0) << "Wrong result from runAsyncCommand(rm)";

    EXPECT_EQ(delilah_console->runAsyncCommand("rm_local /tmp/traces2"), 0) << "Wrong result from runAsyncCommand(rm_local /tmp/traces)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ls_local"), 0) << "Wrong result from runAsyncCommand(ls_local)";

    EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/traces"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/traces)";

    EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/traces -header"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/traces -header)";

    EXPECT_EQ(delilah_console->runAsyncCommand("push_module"), 0) << "Wrong result from runAsyncCommand(push_module)";

    EXPECT_NE(delilah_console->runAsyncCommand("push_module /opt/samson/modules/libtxt.so txt"), 0) << "Wrong result from runAsyncCommand(push_module /opt/samson/modules/libtxt.so txt)";

    EXPECT_NE(delilah_console->runAsyncCommand("ls"), 0) << "Wrong result from runAsyncCommand(ls)";

    EXPECT_NE(delilah_console->runAsyncCommand("ls -group name -rates"), 0) << "Wrong result from runAsyncCommand(ls -group name -rates)";

    EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";

    au::ErrorManager error;
    delilah_console->delilah_disconnect( &error );
    //EXPECT_TRUE(error.isActivated() == false);

    close_delilah_test(delilah_console);
}


