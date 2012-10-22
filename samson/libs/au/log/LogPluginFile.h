
#ifndef _H_AU_LOG_PLUGIN_FILE
#define _H_AU_LOG_PLUGIN_FILE

#include <arpa/inet.h>          // inet_ntoa
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <unistd.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/paBuiltin.h"
#include "parseArgs/paConfig.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"

#include "au/containers/SharedPointer.h"
#include "au/containers/set.h"
#include "au/log/Log.h"
#include "au/log/LogCommon.h"
#include "au/log/LogPlugin.h"
#include "au/network/SocketConnection.h"
#include "au/string/StringUtilities.h"

namespace au {
class LogPluginFile : public LogPlugin {
public:

  LogPluginFile() : LogPlugin("File") {
    set_activated(false);   // No activated by default
  }

  LogPluginFile(std::string local_file);
  virtual ~LogPluginFile() {
  }

  // Emit a log in this channel
  virtual void Emit(au::SharedPointer<Log> log);
  virtual std::string status();

  // Accesors
  std::string local_file() const {
    return local_file_;
  }

  void set_local_file(const std::string& file_name);

  virtual std::string str_info() {
    if (local_file_.length() < 10) {
      return local_file_;
    }

    size_t pos = local_file_.find_last_of("/");
    if (pos == std::string::npos) {
      return local_file_;
    }
    return "..." + local_file_.substr(pos);
  }

private:

  // File name to store logs
  std::string local_file_;

  // Active connections to local file or network connection
  au::SharedPointer<FileDescriptor> local_file_descriptor_;      // Local file descriptor to write the log if not possible to connect
};
}

#endif  // ifndef _H_AU_LOG_CENTRAL