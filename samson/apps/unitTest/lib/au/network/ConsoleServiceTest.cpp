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

#include "au/ErrorManager.h"
#include "au/network/ConsoleService.h"
#include "gtest/gtest.h"


class MyConsoleService : public au::network::ConsoleService {
public:

  MyConsoleService(int port) : au::network::ConsoleService(port) {
  }

  virtual void autoComplete(au::console::ConsoleAutoComplete *info, au::Environment *environment) {
    info->add("start_option");
    info->add("start_option2");
  }

  virtual std::string getPrompt(au::Environment *environment) {
    return "My prompt>>";
  }

  virtual void runCommand(std::string command, au::Environment *environment, au::ErrorManager *error) {
    error->AddMessage(command);
  }
};

#define au_network_ConsoleService_TEST_PORT 9798

TEST(au_network_ConsoleService, DISABLED_basic) {
  // No preivous paralel threads
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());

  // Start a console repetition server
  MyConsoleService console_service(au_network_ConsoleService_TEST_PORT);
  EXPECT_EQ(au::OK, console_service.InitService());

  // Connect with a client
  au::network::ConsoleServiceClientBase client(au_network_ConsoleService_TEST_PORT);
  au::ErrorManager error;
  client.Connect("localhost", &error);
  EXPECT_FALSE(error.IsActivated());

  client.evalCommand("repeat this", &error);
  EXPECT_EQ("repeat this\n", error.GetMessage());

  au::console::ConsoleAutoComplete info("start");
  au::ErrorManager error_auto_complete;
  client.autoComplete(&info);
  EXPECT_EQ(2,  info.getNumAlternatives());

  EXPECT_EQ("My prompt>>", client.getPrompt());

  // Disconnect client
  client.Disconnect(&error);

  // Stop service
  console_service.StopService();

  // No parallel threads at the end
  EXPECT_EQ(0, au::Singleton<au::ThreadManager>::shared()->num_threads());
}
