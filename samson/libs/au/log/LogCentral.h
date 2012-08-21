#ifndef _H_AU_LOG_CENTRAL
#define _H_AU_LOG_CENTRAL

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
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/network/SocketConnection.h"
#include "au/string.h"

#include "au/log/Log.h"
#include "au/log/LogPlugin.h"
#include "log_server_common.h"

namespace au {
  
class LogCentral {

public:

  LogCentral( std::string _host, int _port, std::string _local_file );
  ~LogCentral();

  // In direct mode, we just try to send traces ( not reconnection, no blocking )
  void SetDirectMode(bool flag);

  // Change the host and port
  void SetLogServer(std::string log_host, int log_port = AU_LOG_SERVER_PORT);

  // Write log
  void Write(au::SharedPointer<Log> log);

  // Plugins
  void AddPlugin(LogPlugin *p);
  void RemovePlugin(LogPlugin *p);

  // Accesors
  std::string host() const;
  int port() const;
  std::string local_file() const;
  int getFd() const;
  
private:

  void write_to_server_or_file( au::SharedPointer<Log> log );
  void write_to_plugins( au::SharedPointer<Log> log );

  void close_socket_connection();
  void close_local_file();

  // Mutex to protect socket connection
  au::Token token_;

  // Connection information
  std::string host_;
  int port_;
  std::string local_file_;
  
  SocketConnection *socket_connection_;              // Socket connection with the logServer ( if any )
  au::Cronometer time_since_last_connection_;        // Cronometer with the time since last connection
  size_t time_reconnect_;                            // time for the next reconnection
  
  // Local file descriptor to write the log if not possible to connect
  FileDescriptor *local_file_descriptor_;
  
  // List of plugins
  au::Token token_plugins_;
  au::set<LogPlugin> plugins_;
  
  // Current thread loging
  au::Token token_current_thread_;
  pthread_t current_thread_;
  bool current_thread_activated_;
  
  // Bool direct mode is a non-blocking no-multi-thread no-reconnection way to send logs
  bool direct_mode_;
  
  // Current fd we are using
  int fd_;
  
};
}

#endif  // ifndef _H_AU_LOG_CENTRAL


