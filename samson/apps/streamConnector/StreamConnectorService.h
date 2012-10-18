

#ifndef _H_STREAM_CONNECTOR_SERVICE
#define _H_STREAM_CONNECTOR_SERVICE

#include <set>

#include "logMsg/logMsg.h"

#include "au/console/Console.h"
#include "au/containers/map.h"
#include "au/containers/vector.h"
#include "au/mutex/Token.h"
#include "au/string/string.h"

#include "engine/Buffer.h"

#include "au/network/ConsoleService.h"
#include "au/network/NetworkListener.h"
#include "au/network/NetworkListener.h"
#include "au/network/RESTService.h"

#include "DiskAdaptor.h"
#include "SamsonAdaptor.h"
#include "StreamConnector.h"
#include "common.h"

#include "BufferProcessor.h"

#include "Adaptor.h"
#include "Channel.h"
#include "ConnectorCommand.h"
#include "ConnectorCommand.h"

extern bool interactive;
extern bool run_as_daemon;
extern int sc_console_port;
extern int sc_web_port;


namespace stream_connector {
// Class to accept connection to monitor
class StreamConnectorService : public au::network::ConsoleService {
  StreamConnector *samson_connector;

public:

  StreamConnectorService(StreamConnector *_samson_connector);
  virtual ~StreamConnectorService();

  virtual void runCommand(std::string command, au::Environment *environment, au::ErrorManager *error);
  virtual void autoComplete(au::ConsoleAutoComplete *info, au::Environment *environment);
  virtual std::string getPrompt(au::Environment *environment);
};
}  // End of namespace stream_connector

#endif  // ifndef _H_STREAM_CONNECTOR_SERVICE
