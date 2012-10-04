

#ifndef _H_AU_LOG_PLUGIN_SERVER
#define _H_AU_LOG_PLUGIN_SERVER

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
#include "au/network/SocketConnection.h"
#include "au/string.h"

#include "au/log/Log.h"
#include "au/log/LogPlugin.h"
#include "au/log/LogCommon.h"

namespace au {
  
  /*
   Plugin to connect to a log server
   */
  
  class LogPluginServer : public LogPlugin {
    
  public:
    
    LogPluginServer() : LogPlugin("Server")
    {
      set_activated(false);
    }
    LogPluginServer( const std::string& host, int port, const std::string& local_file );
    ~LogPluginServer();

    void set_host( const std::string& host, int port, const std::string& local_file );
    
    // Change server host
    void SetLogServer( std::string host, int port );
    
    // Emit a log in this channel
    virtual void Emit( au::SharedPointer<Log> log );
    virtual std::string status();
    
    // Accesors
    std::string host() const;
    int port() const;
    std::string local_file() const;
    
  private:
    
    void ReviewSocketConnection(); // Review socket connection
    
    // Connection information
    std::string host_;
    int port_;
    std::string local_file_;
    
    // Active connections to local file or network connection
    au::SharedPointer<FileDescriptor> local_file_descriptor_;    // Local file descriptor to write the log if not possible to connect
    au::SharedPointer<SocketConnection> socket_connection_;      // Socket connection with the logServer ( if any )
    
    au::Cronometer time_since_last_connection_;                  // Cronometer with the time since last connection
    size_t time_reconnect_;                                      // time for the next reconnection
    
  };
  
  
  class LogPluginFile : public LogPlugin {
    
  public:

    LogPluginFile( ) : LogPlugin("File")
    {
      set_activated(false); // No activated by default
    }
    
    LogPluginFile( std::string local_file );
    ~LogPluginFile(){}
    
    // Emit a log in this channel
    virtual void Emit( au::SharedPointer<Log> log );
    virtual std::string status();
    
    // Accesors
    std::string local_file() const
    {
      return local_file_;
    }
    
    void set_local_file( const std::string& file_name );
    
  private:
    
    // File name to store logs
    std::string local_file_;
    
    // Active connections to local file or network connection
    au::SharedPointer<FileDescriptor> local_file_descriptor_;    // Local file descriptor to write the log if not possible to connect
    
  };
  
  
}

#endif  // ifndef _H_AU_LOG_CENTRAL