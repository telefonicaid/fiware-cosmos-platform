/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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

#include "samson/common/ports.h"  // for SAMSON_WORKER_PORT
#include "samson/delilah/Delilah.h"                                     // samson::Delailh
#include "samson/delilah/DelilahConsole.h"

#include "samson/module/KVFormat.h"

#include "logMsg/logMsg.h"

#include "common_delilah_test.h"

/*
 *
 *
 * //Test void GetPrompt();
 * TEST(delilahConsoleTest, GetPrompt)
 * {
 *
 *
 *  samson::DelilahConsole *delilah_console = init_delilah_test();
 *
 *  EXPECT_TRUE(delilah_console != NULL);
 *  EXPECT_EQ(delilah_console->GetPrompt(), "[Unconnected] Delilah>") << "Wrong prompt before connecting";
 *  // Stupid warning on deprecated conversion from string constant to ‘char*’
 *  char *host = strdup("localhost");
 *
 *  int port = SAMSON_WORKER_PORT;
 *  char *env_port = getenv("SAMSON_WORKER_PORT_ENV");
 *  if (env_port != NULL)
 *  {
 *      port = atoi(env_port);
 *  }
 *
 *  char expected_result[1024];
 *  EXPECT_EQ(delilah_console->GetPrompt(), expected_result) << "Wrong prompt after connected (samsonWorker should be running in localhost)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode"), 0) << "Wrong result from (set_mode)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode database"), 0) << "Wrong result from (set_mode database)";
 *  EXPECT_EQ(delilah_console->GetPrompt(), "Database >") << "Wrong prompt after set_mode database";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode logs"), 0) << "Wrong result from (set_mode ogs)";
 *  EXPECT_EQ(delilah_console->GetPrompt(), ">>") << "Wrong prompt after set_mode logs";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from (set_mode normal)";
 *  EXPECT_EQ(delilah_console->GetPrompt(), expected_result) << "Wrong prompt after set_mode normal";
 *
 *  EXPECT_TRUE(error.isActivated() == false);
 *  EXPECT_EQ(delilah_console->GetPrompt(), "[Unconnected] Delilah>") << "Wrong prompt after disconnecting";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";
 *
 *  close_delilah_test(delilah_console);
 *
 * }
 *
 * //Test size_t runAsyncCommand( std::string command);
 * TEST(delilahConsoleTest, runAsyncCommand)
 * {
 *  samson::DelilahConsole *delilah_console = init_delilah_test();
 *
 *  EXPECT_TRUE(delilah_console != NULL);
 *
 *  char *host = strdup("localhost");
 *  int port = SAMSON_WORKER_PORT;
 *  char *env_port = getenv("SAMSON_WORKER_PORT_ENV");
 *  if (env_port != NULL)
 *  {
 *      port = atoi(env_port);
 *  }
 *  char *user = strdup("anonymous");
 *  char *password = strdup("anonymous");
 *
 *  delilah_console->add_alias("test", "ls");
 *  delilah_console->remove_alias("test");
 *  delilah_console->remove_alias("test2");
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("verbose"), 0) << "Wrong result from runAsyncCommand(verbose)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("verbose on"), 0) << "Wrong result from runAsyncCommand(verbose on)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("verbose off"), 0) << "Wrong result from runAsyncCommand(verbose off)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("verbose on"), 0) << "Wrong result from runAsyncCommand(verbose on)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand(""), 0) << "Wrong result from runAsyncCommand(aso)";
 *
 *  delilah_console->add_alias("aso", "add_stream_operation");
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("aso"), 0) << "Wrong result from runAsyncCommand(aso)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("aso txt.parser_words"), 0) << "Wrong result from runAsyncCommand(aso txt.parser_words)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("test_trap"), 0) << "Wrong result from runAsyncCommand(test_trap)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode database"), 0) << "Wrong result from runAsyncCommand(set_mode database)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ls"), 0) << "Wrong result from runAsyncCommand(ls) (in database mode)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode mode_logs"), 0) << "Wrong result from runAsyncCommand(set_mode mode_logs)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from runAsyncCommand(set_mode normal)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode wrong_mode"), 0) << "Wrong result from runAsyncCommand(set_mode wrong_mode)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode logs"), 0) << "Wrong result from runAsyncCommand(set_mode logs)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from runAsyncCommand(set_mode normal) (back to normal)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("connect"), 0) << "Wrong result from runAsyncCommand(connect)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("connect samson01"), 0) << "Wrong result from runAsyncCommand(connect samson01)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("connect samson01 -port 1234"), 0) << "Wrong result from runAsyncCommand(connect samson01 -port 1234)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("history"), 0) << "Wrong result from runAsyncCommand(history)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("history 10"), 0) << "Wrong result from runAsyncCommand(history 10)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("cluster info"), 0) << "Wrong result from runAsyncCommand(cluster info)";
 *
 *  //EXPECT_NE(delilah_console->runAsyncCommand("reload_modules"), 0) << "Wrong result from runAsyncCommand(reload_modules)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help"), 0) << "Wrong result from runAsyncCommand(help)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help category"), 0) << "Wrong result from runAsyncCommand(help category)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help categories"), 0) << "Wrong result from runAsyncCommand(help categories)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help category stream"), 0) << "Wrong result from runAsyncCommand(help category stream)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help stream"), 0) << "Wrong result from runAsyncCommand(help stream)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help cluster"), 0) << "Wrong result from runAsyncCommand(help cluster)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help data"), 0) << "Wrong result from runAsyncCommand(help data)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help delilah"), 0) << "Wrong result from runAsyncCommand(help delilah)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help local"), 0) << "Wrong result from runAsyncCommand(help local)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help modules"), 0) << "Wrong result from runAsyncCommand(help modules)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help push&pop"), 0) << "Wrong result from runAsyncCommand(help push&pop)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help all"), 0) << "Wrong result from runAsyncCommand(help all)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help ls"), 0) << "Wrong result from runAsyncCommand(help ls)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("help ls_stream_operation"), 0) << "Wrong result from runAsyncCommand(help ls_stream_operation)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("threads"), 0) << "Wrong result from runAsyncCommand(threads)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set"), 0) << "Wrong result from runAsyncCommand(set)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set a"), 0) << "Wrong result from runAsyncCommand(set a)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set a b"), 0) << "Wrong result from runAsyncCommand(set a b)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("unset"), 0) << "Wrong result from runAsyncCommand(unset)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("unset a"), 0) << "Wrong result from runAsyncCommand(unset a)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("unset c"), 0) << "Wrong result from runAsyncCommand(unset c)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("example_warning"), 0) << "Wrong result from runAsyncCommand(example_warning)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("local_logs"), 0) << "Wrong result from runAsyncCommand(local_logs)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("local_logs on"), 0) << "Wrong result from runAsyncCommand(local_logs on)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("local_logs off"), 0) << "Wrong result from runAsyncCommand(local_logs off)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("alerts"), 0) << "Wrong result from runAsyncCommand(alerts)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("alerts on"), 0) << "Wrong result from runAsyncCommand(alerts on)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("alerts off"), 0) << "Wrong result from runAsyncCommand(alerts off)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("show_alerts"), 0) << "Wrong result from runAsyncCommand(show_alerts)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("open_alerts_file"), 0) << "Wrong result from runAsyncCommand(open_alerts_file)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("open_alerts_file /tmp/trace.log"), 0) << "Wrong result from runAsyncCommand(open_alerts_file /tmp/trace.log)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("close_alerts_file"), 0) << "Wrong result from runAsyncCommand(close_alerts_file)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("clear_components"), 0) << "Wrong result from runAsyncCommand(clear_components)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ps"), 0) << "Wrong result from runAsyncCommand(ps)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ps 1"), 0) << "Wrong result from runAsyncCommand(ps 1)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ps -clear"), 0) << "Wrong result from runAsyncCommand(ps -clear)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ps clear"), 0) << "Wrong result from runAsyncCommand(ps clear)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ps -clear 3"), 0) << "Wrong result from runAsyncCommand(ps -clear 3)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ps clear 3"), 0) << "Wrong result from runAsyncCommand(ps clear 3)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ps clear wrong_job"), 0) << "Wrong result from runAsyncCommand(ps clear wrong_job)";
 *
 *  LOG_SW(("Before push"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("push"), 0) << "Wrong result from runAsyncCommand(push)";
 *
 *  LOG_SW(("Before push /bin/bash"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("push /bin/bash"), 0) << "Wrong result from runAsyncCommand(push /bin/bash)";
 *
 *  LOG_SW(("Before push /bin/bash a"));
 *  EXPECT_NE(delilah_console->runAsyncCommand("push /bin/bash a"), 0) << "Wrong result from runAsyncCommand(push /bin/bash a)";
 *
 *  LOG_SW(("Before push /tmp/dir_test a"));
 *  EXPECT_NE(delilah_console->runAsyncCommand("push /tmp/dir_test a"), 0) << "Wrong result from runAsyncCommand(push /tmp/dir_test a)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("pop"), 0) << "Wrong result from runAsyncCommand(pop)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("pop a"), 0) << "Wrong result from runAsyncCommand(pop a)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("pop a /tmp/traces"), 0) << "Wrong result from runAsyncCommand(pop a /tmp/traces)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("pop a /tmp/traces2"), 0) << "Wrong result from runAsyncCommand(pop a /tmp/traces2)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("rm"), 0) << "Wrong result from runAsyncCommand(rm)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("rm_local /tmp/traces2"), 0) << "Wrong result from runAsyncCommand(rm_local /tmp/traces)";
 *
 *  //LOG_SW(("Before ls_local"));
 *  //EXPECT_EQ(delilah_console->runAsyncCommand("ls_local"), 0) << "Wrong result from runAsyncCommand(ls_local)";
 *
 *  LOG_SW(("Before show_local_queue"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue"), 0) << "Wrong result from runAsyncCommand(show_local_queue)";
 *
 *  LOG_SW(("Before show_local_queue /tmp/traces"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/traces"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/traces)";
 *
 *  LOG_SW(("Before show_local_queue /tmp/traces -header"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/traces -header"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/traces -header)";
 *
 *  LOG_SW(("Before show_local_queue /tmp/dir_test -header"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/dir_test -header"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/dir_test -header)";
 *
 *  LOG_SW(("Before push_module"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("push_module"), 0) << "Wrong result from runAsyncCommand(push_module)";
 *
 *  //EXPECT_NE(delilah_console->runAsyncCommand("push_module /tmp/libtxt.so txt"), 0) << "Wrong result from runAsyncCommand(push_module /tmp/libtxt.so txt)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_queue_property"), 0) << "Wrong result from runAsyncCommand(set_queue_property)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_queue_property a"), 0) << "Wrong result from runAsyncCommand(set_queue_property a)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_queue_property a pause"), 0) << "Wrong result from runAsyncCommand(set_queue_property a pause)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("unset_queue_property"), 0) << "Wrong result from runAsyncCommand(unset_queue_property)";
 *
 *   EXPECT_EQ(delilah_console->runAsyncCommand("unset_queue_property a"), 0) << "Wrong result from runAsyncCommand(unset_queue_property a)";
 *
 *   EXPECT_NE(delilah_console->runAsyncCommand("unset_queue_property a pause"), 0) << "Wrong result from runAsyncCommand(unset_queue_property a pause)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("unset_queue_property a pause true"), 0) << "Wrong result from runAsyncCommand(unset_queue_property a pause true)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("push_queue"), 0) << "Wrong result from runAsyncCommand(push_queue)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("push_queue a b"), 0) << "Wrong result from runAsyncCommand(push_queue a b)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("pause_queue"), 0) << "Wrong result from runAsyncCommand(pause_queue)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("pause_queue a"), 0) << "Wrong result from runAsyncCommand(pause_queue a)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("play_queue"), 0) << "Wrong result from runAsyncCommand(play_queue)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("play_queue a"), 0) << "Wrong result from runAsyncCommand(play_queue a)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("times"), 0) << "Wrong result from runAsyncCommand(times)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ls_local_queues"), 0) << "Wrong result from runAsyncCommand(ls_local_queues)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("ls_local_queues txt.*"), 0) << "Wrong result from runAsyncCommand(ls_local_queues txt.*)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue"), 0) << "Wrong result from runAsyncCommand(show_local_queue)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("show_local_queue /tmp/traces"), 0) << "Wrong result from runAsyncCommand(show_local_queue /tmp/traces)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("ls"), 0) << "Wrong result from runAsyncCommand(ls)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("set_queue_property a pause false"), 0) << "Wrong result from runAsyncCommand(set_queue_property a pause false)";
 *
 *  EXPECT_NE(delilah_console->runAsyncCommand("ls -group name -rates"), 0) << "Wrong result from runAsyncCommand(ls -group name -rates)";
 *
 *  LOG_SW(("Before disconnect"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("disconnect"), 0) << "Wrong result from runAsyncCommand(disconnect)";
 *
 *  LOG_SW(("Before quit"));
 *  EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";
 *
 * //    au::ErrorManager error;
 * //    delilah_console->delilah_disconnect( &error );
 * //    //EXPECT_TRUE(error.isActivated() == false);
 *
 *  LOG_SW(("Before close_delilah_test()"));
 *  close_delilah_test(delilah_console);
 * }
 *
 * //Test size_t voids( std::string command);
 * TEST(delilahConsoleTest, voids)
 * {
 *  samson::DelilahConsole *delilah_console = init_delilah_test();
 *
 *  EXPECT_TRUE(delilah_console != NULL);
 *
 *  char *host = strdup("localhost");
 *  int port = SAMSON_WORKER_PORT;
 *  char *env_port = getenv("SAMSON_WORKER_PORT_ENV");
 *  if (env_port != NULL)
 *  {
 *      port = atoi(env_port);
 *  }
 *  char *user = strdup("anonymous");
 *  char *password = strdup("anonymous");
 *  LOG_SM(("delilah_console->connect"));
 *  delilah_console->connect( host , port , user , password );
 *
 *  sleep(1);
 *  delilah_console->add_alias("lso", "ls_operations");
 *
 * //    delilah_console->EvalCommand("lso txt.*");
 * //    delilah_console->EvalCommand("set_mode database");
 * //    delilah_console->EvalCommand("ls");
 * //    delilah_console->EvalCommand("set_mode normal");
 * //    delilah_console->EvalCommand("ls_workers");
 * //    delilah_console->EvalCommand("help");
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("lso");
 *      delilah_console->AutoComplete(info);
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("lso txt.*");
 *      delilah_console->AutoComplete(info);
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("test_trap");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set_mode database");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set_mode mode_logs");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set_mode normal");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set_mode wrong_mode");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("history");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("history 10");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("cluster info");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("help");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("help categor");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("help categories cl");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("help all");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("help ls");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("help ls_stream_operation");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("threads");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set a b");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("unset a");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("local_logs");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("alerts");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("alerts on");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("alerts off");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("show_alerts");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("verbose");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("wverbose");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("wverbose 0");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("open_alerts_file /tmp/trace.log");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("close_alerts_file");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("clear_components");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ps");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ps 1");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ps clear");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ps clear 3");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ps clear wrong_job");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("push");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("push /bin/bash");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("push /bin/bash a");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("push /tmp/dir_test a");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("pop");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("pop a");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("pop a /tmp/traces");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("pop a /tmp/traces2");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("rm");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("rm_local /tmp/traces2");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 * //    {
 * //        au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_local");
 * //        delilah_console->AutoComplete(info);
 * //
 * //        delete info;
 * //    }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("show_local_queue");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("show_local_queue /tmp/traces");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("show_local_queue /tmp/traces -header");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("push_module");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("push_module /tmp/libtxt.so txt");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set_queue_property");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set_queue_property a");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set_queue_property a pause");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("set_queue_property a pause true");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls -rat");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls -p");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls -r");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls -group name -rates");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_workers");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_workers -eng");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_workers -d");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_workers -");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_stream_operations");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_stream_operations -p");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_stream_operations -r");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_stream_operations -i");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("ls_stream_operations -o");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("trace");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("wtrace");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("wdebug");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("run");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("run txt");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("run txt.parser_words ");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("init_stream");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("init_stream txt");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("push_module");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *
 *  {
 *      au::console::ConsoleAutoComplete* info = new au::console::ConsoleAutoComplete("quit");
 *      delilah_console->AutoComplete(info);
 *
 *      delete info;
 *  }
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";
 *
 * //    au::ErrorManager error;
 * //    delilah_console->delilah_disconnect( &error );
 * //    EXPECT_TRUE(error.isActivated() == false);
 *
 *  close_delilah_test(delilah_console);
 * }
 *
 * //Test void delilahBase();
 * TEST(delilahConsoleTest, delilahBase)
 * {
 *  samson::DelilahConsole *delilah_console = init_delilah_test();
 *
 *  EXPECT_TRUE(delilah_console != NULL);
 *  EXPECT_EQ(delilah_console->GetPrompt(), "[Unconnected] Delilah>") << "Wrong prompt before connecting";
 *  // Stupid warning on deprecated conversion from string constant to ‘char*’
 *  char *host = strdup("localhost");
 *  int port = SAMSON_WORKER_PORT;
 *  char *env_port = getenv("SAMSON_WORKER_PORT_ENV");
 *  if (env_port != NULL)
 *  {
 *      port = atoi(env_port);
 *  }
 *  char *user = strdup("anonymous");
 *  char *password = strdup("anonymous");
 *  LOG_SM(("delilah_console->connect"));
 *  delilah_console->connect( host , port , user , password );
 *
 *  char expected_result[1024];
 *  sprintf(expected_result, "[%s@%s:%d] Delilah>", user, host, port);
 *  EXPECT_EQ(delilah_console->GetPrompt(), expected_result) << "Wrong prompt after connected (samsonWorker should be running in localhost)";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode database"), 0) << "Wrong result from (set_mode database)";
 *  EXPECT_EQ(delilah_console->GetPrompt(), "Database >") << "Wrong prompt after set_mode database";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode logs"), 0) << "Wrong result from (set_mode ogs)";
 *  EXPECT_EQ(delilah_console->GetPrompt(), ">>") << "Wrong prompt after set_mode logs";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("set_mode normal"), 0) << "Wrong result from (set_mode normal)";
 *  sprintf(expected_result, "[%s@%s:%d] Delilah>", user, host, port);
 *  EXPECT_EQ(delilah_console->GetPrompt(), expected_result) << "Wrong prompt after set_mode normal";
 *
 *  EXPECT_TRUE(strncmp(delilah_console->updateTimeString().c_str(), "Update times from SAMSON elements", strlen("Update times from SAMSON elements")) == 0) << "Wrong updateTimeString at delilahBase";
 *
 *  //delilah_console->updateDelilahXMLString("unitTest");
 *  //delilah_console->updateWorkerXMLString(0, "unitTest");
 *
 * //    EXPECT_EQ(delilah_console->getOperationNames("reduce")[0].c_str(), "OK") << "Wrong reduce getOperationNames at delilahBase";
 * //    EXPECT_EQ(delilah_console->getQueueNames()[0].c_str(), "OK") << "Wrong reduce getQueueNames at delilahBase";
 * //    samson::KVFormat queue_format("*","*");
 * //    EXPECT_EQ(delilah_console->getQueueNames(queue_format)[0].c_str(), "OK") << "Wrong reduce getQueueNames with format at delilahBase";
 * //    EXPECT_EQ(delilah_console->runDatabaseCommand("ls -group name"), "OK") << "Wrong reduce runDatabaseCommand at delilahBase";
 *
 *  EXPECT_EQ(delilah_console->runAsyncCommand("quit"), 0) << "Wrong result from runAsyncCommand(quit)";
 *
 * //    au::ErrorManager error;
 * //    delilah_console->delilah_disconnect( &error );
 * //    EXPECT_TRUE(error.isActivated() == false);
 * //    EXPECT_EQ(delilah_console->GetPrompt(), "[Unconnected] Delilah>") << "Wrong prompt after disconnecting";
 *
 *  close_delilah_test(delilah_console);
 * }
 *
 */
