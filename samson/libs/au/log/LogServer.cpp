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

#include "au/log/LogServer.h"  // Own interface

#include "au/log/LogCommon.h"


namespace au {
LogServer::LogServer()
  : au::network::ConsoleService(LOG_SERVER_DEFAULT_CLIENT_PORT)
    , service_(LOG_SERVER_DEFAULT_PORT, LOG_SERVER_DEFAULT_DIRECTORY) {
  // Init service to receive queries
  Status s = InitService();

  if (s != OK) {
    LM_X(1, ( "Not possible to open query channel on port %d\n", LOG_SERVER_DEFAULT_CLIENT_PORT ));  // Init channel to receive binrary logs
  }
  au::ErrorManager error;
  service_.initLogServerService(&error);

  if (error.IsActivated()) {
    LM_X(1, ( "Not possible to open channel for logs %s\n", error.GetMessage().c_str()));
  }
}

void LogServer::runCommand(std::string command, au::Environment *environment, au::ErrorManager *error) {
  CommandLine cmdLine;

  cmdLine.SetFlagString("format", LOG_DEFAULT_FORMAT_LOG_CLIENT);    // Format of each log
  cmdLine.SetFlagInt("limit", 0);                           // Max number of logs
  cmdLine.SetFlagString("pattern", "");                     // Pattern for strings...
  cmdLine.SetFlagString("time", "");                        // time for logs
  cmdLine.SetFlagString("date", "");                        // date for logs
  cmdLine.SetFlagString("type", "");                        // Filter a particular type of events ( L M W ... )
  cmdLine.SetFlagBoolean("multi_session");                  // Skip new_session marks
  cmdLine.SetFlagBoolean("table");
  cmdLine.SetFlagBoolean("reverse");
  cmdLine.SetFlagBoolean("file");
  cmdLine.SetFlagString("channel", "");
  cmdLine.Parse(command);

  if (cmdLine.get_num_arguments() == 0) {
    error->AddError("No command provided\n");
    return;
  }

  // Get the main command
  std::string main_command = cmdLine.get_argument(0);

  if (main_command == "help") {
    std::string message =
      "---------------------------------------------\n" \
      "Help logClient                               \n" \
      "------------------------------------------------------------------------------------------------------------------\n" \
      "logClient is the console client for a logServer where traces from different systems are collected.\n" \
      "\n \n" \
      "Main commands:\n" \
      "\n \n" \
      " * connect host: Connect to a logServer locate at provided host\n" \
      "\n \n" \
      " * disconnect: Disconnect from a logServer\n" \
      "\n \n" \
      " * info: Show general information about collected logs\n" \
      "\n \n" \
      " * show_channels: Show channels ( log sources )\n" \
      "\n \n" \
      " * show: Show logs on screen\n" \
      "\n \n" \
      "        [-format str_format]   Define format of how logs are displayed on screes   \n" \
      "        [-limit N]             Define the maximum number of logs to be displayed ( default 10000 ) \n" \
      "        [-type T]              Show only logs of a certain type: W M T X V ...\n" \
      "        [-time HH:MM::SS]      Show only logs generated before given time stamp\n" \
      "        [-date DD/MM/YY]       Show only logs generated before given date\n" \
      "        [-pattern str_pattern] Show only logs that match a particular regular experssion\n" \
      "        [-reverse]             Show records in reverse order\n" \
      "        [-multi_session]       Show logs from any session\n" \
      "        [-table]               Show records in a table instead on line by line\n" \
      "\n \n" \
      " * show_connections: Show current connections with this logServer" \
      "\n \n" \
      " * new_session: Create a mark in the logs, so future show commands only show logs starting here.\n" \
      "\n \n" \
      "------------------------------------------------------------------------------------------------------------------\n" \
      "";

    error->AddMessage(message + "\n");

    return;
  }

  if (main_command == "new_session") {
    service_.addNewSession();
    error->AddMessage("OK");
    return;
  }

  if (main_command == "show_connections") {
    std::ostringstream output;

    tables::Table *table = getConnectionsTable();
    table->setTitle("Query connections");
    output << table->str();
    delete table;

    output << "\n";

    table = service_.getConnectionsTable();
    table->setTitle("Log connections");
    output << table->str();
    delete table;
    error->AddMessage(output.str());
    return;
  }


  if (main_command == "info") {
    error->AddMessage(service_.GetInfoTable());
    return;
  }

  // Show channels
  if (main_command == "show_channels") {
    error->AddMessage(service_.GetChannelsTable());
    return;
  }

  // Unknown command error
  error->AddError(au::str("Unknown command %s\n", main_command.c_str()));
}

void LogServer::autoComplete(ConsoleAutoComplete *info, au::Environment *environment) {
  if (info->completingFirstWord()) {
    info->add("help");
    info->add("info");
    info->add("show_channels");
    info->add("show_connections");
    info->add("new_session");
  }

  if (info->completingSecondWord("connect")) {
    info->setHelpMessage("Provide hostname where logServer is located...");
  }

}

std::string LogServer::getPrompt(au::Environment *environment) {
  return au::str("LogServer [%lu logs] >> ", service_.log_container_.size());
}
}