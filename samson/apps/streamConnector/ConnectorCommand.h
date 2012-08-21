
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
    set_flag_boolean("data");
    parse(command);
  }

  bool isDataFlag() {
    return get_flag_bool("data");
  }
};
}

#endif  // ifndef _H_SAMSON_CONNECTOR_COMMAND
