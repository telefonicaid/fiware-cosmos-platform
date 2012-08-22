
#ifndef _H_SAMSON_CONNECTOR_COMMAND
#define _H_SAMSON_CONNECTOR_COMMAND

#include "au/CommandLine.h"

namespace stream_connector {
/*
 *
 * SamsonConnectorCommandLine
 *
 * Command for a samsonConnector instance
 * It is passes acros multiple elements Channel, Item, etc...
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

#endif  // ifndef _H_SAMSON_CONNECTOR_COMMAND
