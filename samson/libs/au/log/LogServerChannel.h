
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

class LogFilterItem {
public:

  LogFilterItem(const std::string&  field, const std::string&  values) {
    field_ = field;
    values_ = au::split(values, ',');

    // Create patterns for all values
    for (size_t i = 0; i < values_.size(); i++) {
      patterns_.push_back(new SimplePattern(values_[i]));
    }
  }

  virtual ~LogFilterItem() {
    patterns_.clearVector();
  }

  virtual bool accept(LogPointer log) {
    std::string value = log->Get(field_);

    for (size_t i = 0; i < patterns_.size(); i++) {
      if (patterns_[i]->match(value)) {
        return true;
      }
    }
    return false;
  }

  static au::SharedPointer<LogFilterItem> Create(const std::string& command) {
    size_t pos = command.find("=");

    if (pos == std::string::npos) {
      return au::SharedPointer<LogFilterItem>(NULL);
    }

    std::string field = command.substr(0, pos);
    std::string values = command.substr(pos + 1);

    return au::SharedPointer<LogFilterItem>(new LogFilterItem(field, values));
  }

private:

  std::string command_;

  std::string field_;
  std::vector<std::string> values_;
  au::vector<SimplePattern> patterns_;
};



class LogFilter {
public:

  LogFilter(const std::string& definition) {
    definition_ = definition;

    // Split in commands
    std::vector<std::string> commands = au::split(definition, '|');
    for (size_t i = 0; i < commands.size(); i++) {
      au::SharedPointer<LogFilterItem> item = LogFilterItem::Create(commands[i]);
      if (item != NULL) {
        items_.push_back(item);
      }
    }
  }

  bool accept(LogPointer log) {
    for (size_t i = 0; i < items_.size(); i++) {
      if (!items_[i]->accept(log)) {
        return false;
      }
    }

    return true;
  }

private:

  std::string definition_;
  std::vector< au::SharedPointer<LogFilterItem> > items_;
};

// Connection to receive logs
class LogProveConnection {
public:

  LogProveConnection(const std::string& filter) : log_filter_(filter) {
    // Filter definition
    // channel_alias=M,W,E
  }

  void Push(LogPointer log) {
    au::TokenTaker tt(&token_);

    if (log_filter_.accept(log)) {
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
  LogFilter log_filter_;
};


class LogServerChannel : public network::Service {
public:

  // Constructor & destructor
  LogServerChannel(int port, std::string _directory);
  virtual ~LogServerChannel();

  // Init service
  void initLogServerChannel(au::ErrorManager *error);

  // network::Service interface : main function for every active connection
  void run(au::SocketConnection *socket_connection, bool *quit);

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

  au::Token token_log_connections_;
  std::set<LogProveConnection *> log_connections_;  // Connection to reveide logs

  friend class LogServer;
};
}
#endif  // ifndef _H_LOG_SERVER_CHANNEL