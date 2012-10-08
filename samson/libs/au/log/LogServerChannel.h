
#ifndef _H_LOG_SERVER_CHANNEL
#define _H_LOG_SERVER_CHANNEL

#include <fcntl.h>
#include <string>
#include <sys/stat.h>  // mkdir

#include "logMsg/logMsg.h"

#include "au/Descriptors.h"
#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/string/split.h"
#include "au/tables/Table.h"

#include "au/log/Log.h"
#include "au/log/LogContainer.h"
#include "au/log/LogFile.h"
#include "au/log/TableLogFormatter.h"

namespace au {
class LogServer;
  
  
  // Connection to receive logs
  class LogProveConnection
  {
    
  public:
    
    
    void Push( LogPointer log )
    {
      au::TokenTaker tt(&token_);
      // Check if we need this log
      // TODO
      
      log_queue_.Push(log);
    }
    
    LogPointer Pop()
    {
      au::TokenTaker tt(&token_);
      return log_queue_.Pop();
    }
    
  private:
    au::Token token_;
    au::Queue<Log> log_queue_;
  };


class LogServerChannel : public network::Service {
public:

  // Constructor & destructor
  LogServerChannel(int port, std::string _directory);
  virtual ~LogServerChannel();

  // Init service
  void initLogServerChannel(au::ErrorManager *error);

  // network::Service interface : main function for every active connection
  void run( au::SocketConnection *socket_connection, bool *quit );

  // Get some info bout logs
  std::string getInfo();

  // Generic function to get a table of logs
  std::string  getTable(au::CommandLine *cmdLine);

  // Generic function to get a table of channels ( log connections )
  std::string  getChannelsTable(au::CommandLine *cmdLine);

  // Add a new session mark ( used in future queries )
  void addNewSession();

private:

  // Open a new file descriptor ( disk ) to save received logs
  void openFileDescriptor(au::ErrorManager *error);

  // Add a log to the channel
  void add(au::SharedPointer<Log> log);

  // Get name for the counter-th log file
  std::string getFileNameForLogFile(int counter);

  au::Token token;               // Mutex protection ( multithread since we receive multiple connections )
  std::string directory;         // Directory to save data in
  int file_counter;              // Used to write over a file
  size_t current_size;           // Current written size
  au::FileDescriptor *fd;        // Current file descriptor to save data
  LogContainer log_container;    // Container of logs in memory ( fast query )
  au::rate::Rate rate;           // Estimated data rate for this channel

  std::set<LogProveConnection*> log_connections_; // Connection to reveide logs
  
  friend class LogServer;
};
}
#endif  // ifndef _H_LOG_SERVER_CHANNEL