
#ifndef _H_SAMSON_CONNECTOR_LOG_MANAGER
#define _H_SAMSON_CONNECTOR_LOG_MANAGER

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/tables/Table.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/Queue.h"

namespace stream_connector {
// Log system for channel - item - connection

class Log {
  
public:

  Log(std::string name, std::string type, std::string message);

  std::string getNameAndMessage();
  std::string getType();
  void writeOnScreen();

  time_t time() const
  {
    return time_;
  }
  
  std::string type() const
  {
    return type_;
  }
  std::string name() const{
    return name_;
  }
  std::string message() const {
    return message_;
  }
  
private:
  
  time_t time_;
  std::string type_;
  std::string name_;
  std::string message_;
  
};


class LogManager {

public:

  LogManager();
  void log( au::SharedPointer<Log> log );
  
  au::tables::Table *getLogsTable(size_t limit);

private:
  
  au::Token token_;
  au::Queue<Log> logs_;
  
};
}  // End of namespace stream_connector

#endif  // ifndef _H_SAMSON_CONNECTOR_LOG_MANAGER
