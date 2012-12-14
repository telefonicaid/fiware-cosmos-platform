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

#ifndef _H_AU_LOG_COUNTER
#define _H_AU_LOG_COUNTER

#include "au/containers/SharedPointer.h"
#include "au/containers/map.h"
#include "au/log/Log.h"
#include "au/log/LogFormatter.h"
#include "au/statistics/Rate.h"
#include "au/string/Descriptors.h"
#include "au/time.h"

namespace au {
class SimpleLogCounter {
public:

  /**
   * \brief SimpleLogCounter : Class to take few statistics about an unbounded streams of logs
   * No log is keep in memory
   */

  SimpleLogCounter() {
    num_ = 0;
    size_ = 0;
    min_time = ( time_t)-1;
    max_time = ( time_t)-1;
  }

  /**
   * \brief  Add another log to take statistics
   */
  void Process(au::LogPointer log);

  /**
   * \brief Get string with time range for processed logs
   */
  std::string GetTimeRange();

  std::string GetTypes() const {
    return types_.str();
  }

  size_t num() const {
    return num_;
  }

  size_t size() const {
    return size_;
  }

private:

  size_t num_;
  size_t size_;
  au::Descriptors types_;
  time_t min_time;
  time_t max_time;
};


/**
 * \brief LogCounter : Class to take a lot of statistics about an unbounded streams of logs
 * No log is keep in memory
 */

class LogCounter {
public:

  LogCounter() {
    Clear();
  }

  ~LogCounter() {
    rates.clearMap();
    types_per_channel_.clearMap();
    descriptors_.clearMap();
    logs_per_channel_.clearMap();
  }

  // Process a new log to count
  void Process(LogPointer log);

  // Clear information
  void Clear();

  // Accessors
  std::string str(int c) const;
  std::string str_rate(int c) const;
  std::string str_types_per_channel(int c) const;
  std::string str_global() const;
  std::string GetInfoTable() const;
  std::string GetChannelsTable() const;
  size_t logs() const;
  size_t size() const;
  size_t rate() const;

private:

  au::Rate rate_;                                 // total rate of logs/second
  au::map<int, au::Rate > rates;                  // Rate per channel
  au::map<int, au::Descriptors > types_per_channel_;  // Type of message per channel

  // global descriptor information
  au::map<std::string, au::Descriptors> descriptors_;

  // Info for channels
  au::map<std::string, SimpleLogCounter> logs_per_channel_;

  // Min and max observed time-stamps
  time_t min_time_;
  time_t max_time_;
};
}

#endif  // ifndef _H_AU_LOG_COUNTER