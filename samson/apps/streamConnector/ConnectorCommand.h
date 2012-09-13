
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
