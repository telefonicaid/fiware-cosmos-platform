
#ifndef _H_LOG_SERVER
#define _H_LOG_SERVER

#include <fcntl.h>
#include <string>
#include <sys/stat.h>  // mkdir

#include "logMsg/logMsg.h"

#include "au/containers/list.h"
#include "au/containers/map.h"
#include "au/log/Log.h"
#include "au/log/LogServerChannel.h"
#include "au/network/ConsoleService.h"
#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/string/string.h"
#include "au/tables/Table.h"

namespace au {
class LogServerChannel;

class LogServer : au::network::ConsoleService {
public:

  // Constructor
  LogServer();
  ~LogServer() {
  };

  // au::network::ConsoleService
  void runCommand(std::string command, au::Environment *environment, au::ErrorManager *error);
  void autoComplete(ConsoleAutoComplete *info, au::Environment *environment);
  std::string getPrompt(au::Environment *environment);

private:

  LogServerChannel channel;   // Channel to accept connection with binary logs
};
}
#endif  // ifndef _H_LOG_SERVER