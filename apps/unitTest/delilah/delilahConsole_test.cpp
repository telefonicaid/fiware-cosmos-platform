/* ****************************************************************************
 *
 * FILE            delilahConsole_test.cpp
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

#include "samson/module/KVFormat.h"

#include "logMsg/logMsg.h"

#include "common_delilah_test.h"


//Test void getPrompt();
TEST(delilahConsoleTest, getPrompt)
{
    samson::DelilahConsole *delilah_console = init_delilah_test();

    EXPECT_TRUE(delilah_console != NULL);
    EXPECT_EQ(delilah_console->getPrompt(), "[Unconnected] Delilah>") << "Wrong prompt before connecting";
    // Stupid warning on deprecated conversion from string constant to ‘char*’
    char *host = strdup("localhost");
    int port = SAMSON_WORKER_PORT;
    char *user = strdup("anonymous");
    char *password = strdup("anonymous");
    LM_M(("delilah_console->connect"));
    delilah_console->connect( host , port , user , password );

    char expected_result[1024];
    sprintf(expected_result, "[%s@%s:%d] Delilah>", user, host, port);
    EXPECT_EQ(delilah_console->getPrompt(), expected_result) << "Wrong prompt after connected (samsonWorker should be running in localhost)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode"), 0) << "Wrong result from (set_mode)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode database"), 0) << "Wrong result from (set_mode database)";
    EXPECT_EQ(delilah_console->getPrompt(), "Database >") << "Wrong prompt after set_mode database";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode logs"), 0) << "Wrong result from (set_mode ogs)";
    EXPECT_EQ(delilah_console->getPrompt(), ">>") << "Wrong prompt after set_mode logs";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from (set_mode normal)";
    sprintf(expected_result, "[%s@%s:%d] Delilah>", user, host, port);
    EXPECT_EQ(delilah_console->getPrompt(), expected_result) << "Wrong prompt after set_mode normal";

    EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";

//    au::ErrorManager error;
//    delilah_console->delilah_disconnect( &error );
//    EXPECT_TRUE(error.isActivated() == false);
//    EXPECT_EQ(delilah_console->getPrompt(), "[Unconnected] Delilah2>") << "Wrong prompt after disconnecting";
//
//    delilah_console->connect( host , port , user , password );

    EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";

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

    delilah_console->add_alias("test", "ls");
    delilah_console->remove_alias("test");
    delilah_console->remove_alias("test2");

    EXPECT_EQ(delilah_console->runAsyncCommand(""), 0) << "Wrong result from runAsyncCommand(aso)";

    delilah_console->add_alias("aso", "add_stream_operation");

    EXPECT_NE(delilah_console->runAsyncCommand("aso"), 0) << "Wrong result from runAsyncCommand(aso)";

    EXPECT_NE(delilah_console->runAsyncCommand("aso txt.parser_words"), 0) << "Wrong result from runAsyncCommand(aso txt.parser_words)";

    EXPECT_NE(delilah_console->runAsyncCommand("test_trap"), 0) << "Wrong result from runAsyncCommand(test_trap)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode database"), 0) << "Wrong result from runAsyncCommand(set_mode database)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ls"), 0) << "Wrong result from runAsyncCommand(ls) (in database mode)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode mode_logs"), 0) << "Wrong result from runAsyncCommand(set_mode mode_logs)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from runAsyncCommand(set_mode normal)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode wrong_mode"), 0) << "Wrong result from runAsyncCommand(set_mode wrong_mode)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode logs"), 0) << "Wrong result from runAsyncCommand(set_mode logs)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from runAsyncCommand(set_mode normal) (back to normal)";

    EXPECT_EQ(delilah_console->runAsyncCommand("connect"), 0) << "Wrong result from runAsyncCommand(connect)";

    EXPECT_EQ(delilah_console->runAsyncCommand("connect samson01"), 0) << "Wrong result from runAsyncCommand(connect samson01)";

    EXPECT_EQ(delilah_console->runAsyncCommand("connect samson01 -port 1234"), 0) << "Wrong result from runAsyncCommand(connect samson01 -port 1234)";

    EXPECT_EQ(delilah_console->runAsyncCommand("history"), 0) << "Wrong result from runAsyncCommand(history)";

    EXPECT_EQ(delilah_console->runAsyncCommand("history 10"), 0) << "Wrong result from runAsyncCommand(history 10)";

    EXPECT_EQ(delilah_console->runAsyncCommand("cluster info"), 0) << "Wrong result from runAsyncCommand(cluster info)";

    //EXPECT_NE(delilah_console->runAsyncCommand("reload_modules"), 0) << "Wrong result from runAsyncCommand(reload_modules)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help"), 0) << "Wrong result from runAsyncCommand(help)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help category"), 0) << "Wrong result from runAsyncCommand(help category)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help categories"), 0) << "Wrong result from runAsyncCommand(help categories)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help category stream"), 0) << "Wrong result from runAsyncCommand(help category stream)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help stream"), 0) << "Wrong result from runAsyncCommand(help stream)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help cluster"), 0) << "Wrong result from runAsyncCommand(help cluster)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help data"), 0) << "Wrong result from runAsyncCommand(help data)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help delilah"), 0) << "Wrong result from runAsyncCommand(help delilah)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help local"), 0) << "Wrong result from runAsyncCommand(help local)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help modules"), 0) << "Wrong result from runAsyncCommand(help modules)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help push&pop"), 0) << "Wrong result from runAsyncCommand(help push&pop)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help all"), 0) << "Wrong result from runAsyncCommand(help all)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help ls"), 0) << "Wrong result from runAsyncCommand(help ls)";

    EXPECT_EQ(delilah_console->runAsyncCommand("help ls_stream_operation"), 0) << "Wrong result from runAsyncCommand(help ls_stream_operation)";

    EXPECT_EQ(delilah_console->runAsyncCommand("threads"), 0) << "Wrong result from runAsyncCommand(threads)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set"), 0) << "Wrong result from runAsyncCommand(set)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set a"), 0) << "Wrong result from runAsyncCommand(set a)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set a b"), 0) << "Wrong result from runAsyncCommand(set a b)";

    EXPECT_EQ(delilah_console->runAsyncCommand("unset"), 0) << "Wrong result from runAsyncCommand(unset)";

    EXPECT_EQ(delilah_console->runAsyncCommand("unset a"), 0) << "Wrong result from runAsyncCommand(unset a)";

    EXPECT_EQ(delilah_console->runAsyncCommand("unset c"), 0) << "Wrong result from runAsyncCommand(unset c)";

    EXPECT_EQ(delilah_console->runAsyncCommand("example_warning"), 0) << "Wrong result from runAsyncCommand(example_warning)";

    EXPECT_EQ(delilah_console->runAsyncCommand("local_logs"), 0) << "Wrong result from runAsyncCommand(local_logs)";

    EXPECT_EQ(delilah_console->runAsyncCommand("local_logs on"), 0) << "Wrong result from runAsyncCommand(local_logs on)";

    EXPECT_EQ(delilah_console->runAsyncCommand("local_logs off"), 0) << "Wrong result from runAsyncCommand(local_logs off)";

    EXPECT_EQ(delilah_console->runAsyncCommand("alerts"), 0) << "Wrong result from runAsyncCommand(alerts)";

    EXPECT_EQ(delilah_console->runAsyncCommand("alerts on"), 0) << "Wrong result from runAsyncCommand(alerts on)";

    EXPECT_EQ(delilah_console->runAsyncCommand("alerts off"), 0) << "Wrong result from runAsyncCommand(alerts off)";

    EXPECT_EQ(delilah_console->runAsyncCommand("show_alerts"), 0) << "Wrong result from runAsyncCommand(show_alerts)";

    EXPECT_EQ(delilah_console->runAsyncCommand("verbose"), 0) << "Wrong result from runAsyncCommand(verbose)";

    EXPECT_EQ(delilah_console->runAsyncCommand("verbose on"), 0) << "Wrong result from runAsyncCommand(verbose on)";

    EXPECT_EQ(delilah_console->runAsyncCommand("verbose off"), 0) << "Wrong result from runAsyncCommand(verbose off)";

    EXPECT_EQ(delilah_console->runAsyncCommand("verbose on"), 0) << "Wrong result from runAsyncCommand(verbose on)";

    EXPECT_EQ(delilah_console->runAsyncCommand("open_alerts_file"), 0) << "Wrong result from runAsyncCommand(open_alerts_file)";

    EXPECT_EQ(delilah_console->runAsyncCommand("open_alerts_file /tmp/trace.log"), 0) << "Wrong result from runAsyncCommand(open_alerts_file /tmp/trace.log)";

    EXPECT_EQ(delilah_console->runAsyncCommand("close_alerts_file"), 0) << "Wrong result from runAsyncCommand(close_alerts_file)";

    EXPECT_EQ(delilah_console->runAsyncCommand("clear_components"), 0) << "Wrong result from runAsyncCommand(clear_components)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps"), 0) << "Wrong result from runAsyncCommand(ps)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps 1"), 0) << "Wrong result from runAsyncCommand(ps 1)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps -clear"), 0) << "Wrong result from runAsyncCommand(ps -clear)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps clear"), 0) << "Wrong result from runAsyncCommand(ps clear)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps -clear 3"), 0) << "Wrong result from runAsyncCommand(ps -clear 3)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps clear 3"), 0) << "Wrong result from runAsyncCommand(ps clear 3)";

    EXPECT_EQ(delilah_console->runAsyncCommand("ps clear wrong_job"), 0) << "Wrong result from runAsyncCommand(ps clear wrong_job)";

    LM_W(("Before push"));
    EXPECT_EQ(delilah_console->runAsyncCommand("push"), 0) << "Wrong result from runAsyncCommand(push)";

    LM_W(("Before push /bin/bash"));
    EXPECT_EQ(delilah_console->runAsyncCommand("push /bin/bash"), 0) << "Wrong result from runAsyncCommand(push /bin/bash)";

    LM_W(("Before push /bin/bash a"));
    EXPECT_NE(delilah_console->runAsyncCommand("push /bin/bash a"), 0) << "Wrong result from runAsyncCommand(push /bin/bash a)";

    LM_W(("Before push /tmp/dir_test a"));
    EXPECT_NE(delilah_console->runAsyncCommand("push /tmp/dir_test a"), 0) << "Wrong result from runAsyncCommand(push /tmp/dir_test a)";

    EXPECT_EQ(delilah_console->runAsyncCommand("pop"), 0) << "Wrong result from runAsyncCommand(pop)";

    EXPECT_EQ(delilah_console->runAsyncCommand("pop a"), 0) << "Wrong result from runAsyncCommand(pop a)";

    EXPECT_NE(delilah_console->runAsyncCommand("pop a /tmp/traces"), 0) << "Wrong result from runAsyncCommand(pop a /tmp/traces)";

    EXPECT_NE(delilah_console->runAsyncCommand("pop a /tmp/traces2"), 0) << "Wrong result from runAsyncCommand(pop a /tmp/traces2)";

    EXPECT_EQ(delilah_console->runAsyncCommand("rm"), 0) << "Wrong result from runAsyncCommand(rm)";

    EXPECT_EQ(delilah_console->runAsyncCommand("rm_local /tmp/traces2"), 0) << "Wrong result from runAsyncCommand(rm_local /tmp/traces)";

    LM_W(("Before ls_local"));
    EXPECT_EQ(delilah_console->runAsyncCommand("ls_local"), 0) << "Wrong result from runAsyncCommand(ls_local)";

    LM_W(("Before show_local_queue"));
    EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue"), 0) << "Wrong result from runAsyncCommand(show_local_queue)";

    LM_W(("Before show_local_queue /tmp/traces"));
    EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/traces"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/traces)";

    EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/traces -header"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/traces -header)";

    EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/dir_test -header"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/dir_test -header)";

    EXPECT_EQ(delilah_console->runAsyncCommand("push_module"), 0) << "Wrong result from runAsyncCommand(push_module)";

    //EXPECT_NE(delilah_console->runAsyncCommand("push_module /tmp/libtxt.so txt"), 0) << "Wrong result from runAsyncCommand(push_module /tmp/libtxt.so txt)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_queue_property"), 0) << "Wrong result from runAsyncCommand(set_queue_property)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_queue_property a"), 0) << "Wrong result from runAsyncCommand(set_queue_property a)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_queue_property a pause"), 0) << "Wrong result from runAsyncCommand(set_queue_property a pause)";

    EXPECT_NE(delilah_console->runAsyncCommand("set_queue_property a pause true"), 0) << "Wrong result from runAsyncCommand(set_queue_property a pause true)";

    EXPECT_NE(delilah_console->runAsyncCommand("ls"), 0) << "Wrong result from runAsyncCommand(ls)";

    EXPECT_NE(delilah_console->runAsyncCommand("set_queue_property a pause false"), 0) << "Wrong result from runAsyncCommand(set_queue_property a pause false)";

    EXPECT_NE(delilah_console->runAsyncCommand("ls -group name -rates"), 0) << "Wrong result from runAsyncCommand(ls -group name -rates)";

    EXPECT_EQ(delilah_console->runAsyncCommand("disconnect"), 0) << "Wrong result from runAsyncCommand(disconnect)";

    EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";

//    au::ErrorManager error;
//    delilah_console->delilah_disconnect( &error );
//    //EXPECT_TRUE(error.isActivated() == false);

    close_delilah_test(delilah_console);
}

//Test size_t voids( std::string command);
TEST(delilahConsoleTest, voids)
{
    samson::DelilahConsole *delilah_console = init_delilah_test();

    EXPECT_TRUE(delilah_console != NULL);

    char *host = strdup("localhost");
    int port = SAMSON_WORKER_PORT;
    char *user = strdup("anonymous");
    char *password = strdup("anonymous");
    LM_M(("delilah_console->connect"));
    delilah_console->connect( host , port , user , password );

    sleep(1);
    delilah_console->add_alias("lso", "ls_operations");

//    delilah_console->evalCommand("lso txt.*");
//    delilah_console->evalCommand("set_mode database");
//    delilah_console->evalCommand("ls");
//    delilah_console->evalCommand("set_mode normal");
//    delilah_console->evalCommand("ls_workers");
//    delilah_console->evalCommand("help");

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("lso");
        delilah_console->autoComplete(info);
        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("lso txt.*");
        delilah_console->autoComplete(info);
        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("test_trap");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set_mode database");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set_mode mode_logs");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set_mode normal");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set_mode wrong_mode");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("history");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("history 10");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("cluster info");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("help");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("help categor");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("help categories cl");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("help all");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("help ls");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("help ls_stream_operation");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("threads");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set a b");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("unset a");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("local_logs");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("alerts");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("alerts on");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("alerts off");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("show_alerts");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("verbose");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("wverbose");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("wverbose 0");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("open_alerts_file /tmp/trace.log");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("close_alerts_file");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("clear_components");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ps");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ps 1");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ps clear");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ps clear 3");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ps clear wrong_job");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("push");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("push /bin/bash");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("push /bin/bash a");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("push /tmp/dir_test a");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("pop");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("pop a");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("pop a /tmp/traces");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("pop a /tmp/traces2");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("rm");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("rm_local /tmp/traces2");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_local");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("show_local_queue");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("show_local_queue /tmp/traces");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("show_local_queue /tmp/traces -header");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("push_module");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("push_module /tmp/libtxt.so txt");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set_queue_property");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set_queue_property a");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set_queue_property a pause");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("set_queue_property a pause true");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls -rat");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls -p");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls -r");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls -group name -rates");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_workers");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_workers -eng");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_workers -d");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_workers -");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_stream_operations");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_stream_operations -p");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_stream_operations -r");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_stream_operations -i");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("ls_stream_operations -o");
        delilah_console->autoComplete(info);

        delete info;
    }

    {
        au::ConsoleAutoComplete* info = new au::ConsoleAutoComplete("quit");
        delilah_console->autoComplete(info);

        delete info;
    }

    EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";

//    au::ErrorManager error;
//    delilah_console->delilah_disconnect( &error );
//    EXPECT_TRUE(error.isActivated() == false);

    close_delilah_test(delilah_console);
}

//Test void delilahBase();
TEST(delilahConsoleTest, delilahBase)
{
    samson::DelilahConsole *delilah_console = init_delilah_test();

    EXPECT_TRUE(delilah_console != NULL);
    EXPECT_EQ(delilah_console->getPrompt(), "[Unconnected] Delilah>") << "Wrong prompt before connecting";
    // Stupid warning on deprecated conversion from string constant to ‘char*’
    char *host = strdup("localhost");
    int port = SAMSON_WORKER_PORT;
    char *user = strdup("anonymous");
    char *password = strdup("anonymous");
    LM_M(("delilah_console->connect"));
    delilah_console->connect( host , port , user , password );

    char expected_result[1024];
    sprintf(expected_result, "[%s@%s:%d] Delilah>", user, host, port);
    EXPECT_EQ(delilah_console->getPrompt(), expected_result) << "Wrong prompt after connected (samsonWorker should be running in localhost)";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode database"), 0) << "Wrong result from (set_mode database)";
    EXPECT_EQ(delilah_console->getPrompt(), "Database >") << "Wrong prompt after set_mode database";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode logs"), 0) << "Wrong result from (set_mode ogs)";
    EXPECT_EQ(delilah_console->getPrompt(), ">>") << "Wrong prompt after set_mode logs";

    EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from (set_mode normal)";
    sprintf(expected_result, "[%s@%s:%d] Delilah>", user, host, port);
    EXPECT_EQ(delilah_console->getPrompt(), expected_result) << "Wrong prompt after set_mode normal";

    EXPECT_TRUE(strncmp(delilah_console->updateTimeString().c_str(), "Update times from SAMSON elements", strlen("Update times from SAMSON elements")) == 0) << "Wrong updateTimeString at delilahBase";

    //delilah_console->updateDelilahXMLString("unitTest");
    //delilah_console->updateWorkerXMLString(0, "unitTest");

//    EXPECT_EQ(delilah_console->getOperationNames("reduce")[0].c_str(), "OK") << "Wrong reduce getOperationNames at delilahBase";
//    EXPECT_EQ(delilah_console->getQueueNames()[0].c_str(), "OK") << "Wrong reduce getQueueNames at delilahBase";
//    samson::KVFormat queue_format("*","*");
//    EXPECT_EQ(delilah_console->getQueueNames(queue_format)[0].c_str(), "OK") << "Wrong reduce getQueueNames with format at delilahBase";
//    EXPECT_EQ(delilah_console->runDatabaseCommand("ls -group name"), "OK") << "Wrong reduce runDatabaseCommand at delilahBase";

    EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";

//    au::ErrorManager error;
//    delilah_console->delilah_disconnect( &error );
//    EXPECT_TRUE(error.isActivated() == false);
//    EXPECT_EQ(delilah_console->getPrompt(), "[Unconnected] Delilah>") << "Wrong prompt after disconnecting";

    close_delilah_test(delilah_console);
}

