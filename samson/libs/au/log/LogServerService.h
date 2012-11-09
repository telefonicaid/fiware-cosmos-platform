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

#ifndef _H_LOG_SERVER_CHANNEL
#define _H_LOG_SERVER_CHANNEL

#include <fcntl.h>
#include <string>
#include <sys/stat.h>  // mkdir

#include "logMsg/logMsg.h"

#include "au/string/Descriptors.h"
#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/string/StringUtilities.h"
#include "au/tables/Table.h"
#include "au/log/Log.h"
#include "au/log/LogFilter.h"
#include "au/log/LogContainer.h"


/*
 
 LogServerService
 
 Network service of logServer
 
 */

namespace au {
class LogServer;
class LogQuery;
  namespace gpb
  {
    class LogConnectionHello;
  }
  
// Connection to receive logs
class LogProbeConnection {
public:

  LogProbeConnection(const std::string& filter) {
    au::ErrorManager error;
    log_filter_ = LogFilter::Create(filter,error); // Ignore error
    
  }

  void Push(LogPointer log) {
    au::TokenTaker tt(&token_);
    if ((log_filter_ == NULL ) || (log_filter_->Accept(log))) {
      log_queue_.Push(log);
    }
  }

  LogPointer Pop() {
    au::TokenTaker tt(&token_);

    return log_queue_.Pop();
  }

private:

  au::Token token_;
  au::Queue<Log> log_queue_;
  LogFilterPointer log_filter_;
};

  
class LogServerService : public network::Service {
public:

  // Constructor & destructor
  LogServerService(int port, const std::string& _directory);
  virtual ~LogServerService();

  // Init service
  void initLogServerService(au::ErrorManager *error);

  // network::Service interface : main function for every active connection
  void run(au::SocketConnection *socket_connection, bool *quit);

  // Get some info bout logs
  std::string GetInfoTable();

  // Generic function to get a table of channels ( log connections )
  std::string  GetChannelsTable();

  // Add a new session mark ( used in future queries )
  void addNewSession();

  // Process a query over logs
  void Process( LogQuery* log_query );
  
private:

  // Open a new file descriptor ( disk ) to save received logs
  void openFileDescriptor(au::ErrorManager *error);

  // Add a log to the channel
  void add(au::SharedPointer<Log> log);

  // Get name for the counter-th log file
  std::string getFileNameForLogFile(int counter);

  // Specific functions for log provider / probe or query
  void runLogProvider(gpb::LogConnectionHello* hello, au::SocketConnection *socket_connection, bool *quit);
  void runLogProbe(gpb::LogConnectionHello* hello,au::SocketConnection *socket_connection, bool *quit);
  void runLogQuery(gpb::LogConnectionHello* hello,au::SocketConnection *socket_connection, bool *quit);

  
  au::Token token;               // Mutex protection ( multithread since we receive multiple connections )
  LogContainer log_container_;    // Container of logs in memory ( fast query )
  std::string directory;         // Directory to save data in
  int file_counter;              // Used to write over a file
  size_t current_size;           // Current written size
  au::FileDescriptor *fd;        // Current file descriptor to save data
  au::rate::Rate rate;           // Estimated data rate for this channel

  au::Token token_log_probe_connections_;
  std::set<LogProbeConnection *> log_probe_connections_;  // Connection to reveide logs

  friend class LogServer;
};
}
#endif  // ifndef _H_LOG_SERVER_CHANNEL