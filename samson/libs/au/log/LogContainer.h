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
#ifndef _H_LOG_LOG_CONTAINER
#define _H_LOG_LOG_CONTAINER

#include <fcntl.h>
#include <string>
#include <sys/stat.h>  // mkdir

#include "logMsg/logMsg.h"

#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"
#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/string/Descriptors.h"
#include "au/string/StringUtilities.h"
#include "au/tables/Table.h"

namespace au {
class LogServer;
class LogQuery;

class LogContainer {
public:

  /**
   * \brief On-memory container of logs to process queries of previous logs
   * Used in LogServer to keep in memory last logs
   */

  LogContainer();
  ~LogContainer() {
  };

  void Push(au::SharedPointer<Log> log);
  void Clear();

  // Accesor for size
  size_t size() const;

  // Process a particular query
  void Process(LogQuery *log_query);

  // Get access to counters
  LogCounter& log_counter() {
    return log_counter_;
  }

private:

  mutable au::Token token_;
  LogCounter log_counter_;
  std::list<LogPointer> logs_;
  size_t max_num_logs_;
};
}

#endif  // ifndef _H_LOG_LOG_CONTAINER