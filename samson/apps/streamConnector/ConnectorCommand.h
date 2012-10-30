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

#ifndef _H_STREAM_CONNECTOR_COMMAND
#define _H_STREAM_CONNECTOR_COMMAND

#include "au/CommandLine.h"

namespace stream_connector {
/*
 *
 * StreamConnectorCommandLine
 *
 * Command for a streamConnector instance
 * It is passes across multiple elements Channel, Item, etc...
 *
 */

class CommandLine : public au::CommandLine {
public:

  CommandLine(std::string command) {
    SetFlagBoolean("data");
    Parse(command);
  }

  bool isDataFlag() {
    return GetFlagBool("data");
  }
};
}

#endif  // ifndef _H_STREAM_CONNECTOR_COMMAND
