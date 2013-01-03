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

#ifndef _H_LOG_SERVER
#define _H_LOG_SERVER

#include <fcntl.h>
#include <string>
#include <sys/stat.h>  // mkdir

#include "logMsg/logMsg.h"

#include "au/containers/list.h"
#include "au/containers/map.h"
#include "au/log/Log.h"
#include "au/log/LogServerService.h"
#include "au/network/ConsoleService.h"
#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/string/StringUtilities.h"
#include "au/tables/Table.h"

/*
 *
 * Main class of the logServer program
 *
 * It is basically a Console-based remote server with a dedicated service to receive:
 * - log-providers
 * - log-probes
 * - log-queries
 *
 */


namespace au {
class LogServerService;

class LogServer : au::network::ConsoleService {
public:

  // Constructor
  LogServer();
  ~LogServer() {
  };

  // au::network::ConsoleService
  void runCommand(std::string command, au::Environment *environment, au::ErrorManager *error);
  void AutoComplete(console::ConsoleAutoComplete *info, au::Environment *environment);
  std::string GetPrompt(au::Environment *environment);

private:
  LogServerService service_;   // Service to accept connection with binary logs ( providers, probes, queries...)
};
}
#endif  // ifndef _H_LOG_SERVER
