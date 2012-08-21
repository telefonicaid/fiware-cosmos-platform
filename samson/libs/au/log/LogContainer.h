#ifndef _H_LOG_LOG_CONTAINER
#define _H_LOG_LOG_CONTAINER

#include <fcntl.h>
#include <string>
#include <sys/stat.h>  // mkdir

#include "logMsg/logMsg.h"

#include "au/Descriptors.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/Queue.h"
#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/string/split.h"
#include "au/tables/Table.h"

#include "au/log/Log.h"
#include "au/log/LogFile.h"
#include "au/log/TableLogFormatter.h"

namespace au {
  class LogServer;
  
  class LogContainer {
    
  public:
    
    LogContainer();
    ~LogContainer(){};
    
    void Push( au::SharedPointer<Log> log);
    void Clear();
    
    // Accesor for size
    size_t size() const;
    
    // Complete info string
    std::string getInfo() const;

    // Get vector with all instances
    std::vector< au::SharedPointer<Log> > logs();
    
  private:
    
    mutable au::Token token_;
    au::Queue<Log> logs_;
    size_t max_num_logs_;
    
  };
}

#endif