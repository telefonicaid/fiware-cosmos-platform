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
#ifndef _H_AU_NETWORK_CONSOLE_SERVICE
#define _H_AU_NETWORK_CONSOLE_SERVICE

#include <string>

#include "au/Status.h"
#include "au/containers/set.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/tables/Table.h"

#include "au/Environment.h"
#include "au/console/Console.h"
#include "au/gpb.h"
#include "au/network/Service.h"
namespace au {
namespace gpb {
class ConsolePacket;
}
}

namespace au {
namespace network {
class Service;

class ConsoleServiceClientBase {
public:

  ConsoleServiceClientBase(int port);

  void Connect(std::string host, au::ErrorManager *error);
  void Disconnect(au::ErrorManager *error);

  bool Write(au::gpb::ConsolePacket *packet, au::ErrorManager *error);
  bool Read(au::gpb::ConsolePacket **packet, au::ErrorManager *error);

  // Methods related with au::console::Console
  std::string getPrompt();
  void evalCommand(const std::string& command, au::ErrorManager *error);
  virtual void autoComplete(console::ConsoleAutoComplete *info);
  void addEspaceSequence(std::string sequence);
  virtual void process_escape_sequence(std::string sequence) {
  };

private:

  // Full a message to be sent
  void FillMessage(au::gpb::ConsolePacket *message, au::ErrorManager *error);

  int port_;
  SocketConnection *socket_connection_;

  // Prompt request delayed
  au::Cronometer cronometer_prompt_request_;
  std::string current_prompt_;
};

// Simple console to interact with the client

class ConsoleServiceClient : public ConsoleServiceClientBase, public console::Console {
public:

  ConsoleServiceClient(int port);

  // Virtual methods of console
  virtual void evalCommand(const std::string& command);
  virtual void autoComplete(console::ConsoleAutoComplete *info);
  virtual std::string getPrompt();
};

// Service based on a remote console

class ConsoleService : public Service {
public:

  ConsoleService(int port) :
    Service(port) {
  }

  // Virtual methods to be implemented by subclasses
  virtual void runCommand(std::string command, au::Environment *environment, au::ErrorManager *error) {
  }

  virtual void autoComplete(console::ConsoleAutoComplete *info, au::Environment *environment) {
  }

  virtual std::string getPrompt(au::Environment *environment) {
    return ">>";
  }

private:

  // main function executed by all threads
  virtual void run(SocketConnection *socket_connection, bool *quit);

  // Fill message to be sent to client
  void fill_message(au::ErrorManager *error, au::gpb::ConsolePacket *message);
};
}
}

#endif  // ifndef _H_AU_NETWORK_CONSOLE_SERVICE
