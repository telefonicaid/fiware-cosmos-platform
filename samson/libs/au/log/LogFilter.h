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


#ifndef AU_LOG_FILTER_
#define AU_LOG_FILTER_

#include <vector>

#include "au/ErrorManager.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/vector.h"
#include "au/log/Log.h"


/*
 *
 * LogFilter
 *
 * Generic class to filter incomming logs based on different criteria like
 *
 * - Number of logs
 * - Time refernce
 * - General pattern matching with all fields
 * - Specific pattern matching with a provided field
 *
 */

namespace au {
class LogFilterItem {
public:
  LogFilterItem() {
  }

  virtual ~LogFilterItem() {
  };
  virtual bool Accept(LogPointer log) = 0;  // Virtual method to filter incomming logs
  // General method to create items
  static au::SharedPointer<LogFilterItem> Create(const std::string& command, au::ErrorManager& error);
};


/*
 * Filter based on the number of recevied logs
 */

class LogFilterItemTimeLimit : public LogFilterItem {
public:

  LogFilterItemTimeLimit(size_t limit) {
    count_ = 0;
    limit_ = limit;
  }

  virtual bool Accept(LogPointer log) {
    return ( ++count_ <= limit_ );
  }

private:

  size_t limit_;
  size_t count_;
};

/*
 * Filter based on a time reference
 * Only logs generated latter than X are accepted
 */

class LogFilterItemTimeRef : public LogFilterItem {
public:

  LogFilterItemTimeRef(size_t time_ref) {
    time_ref_ = time_ref;
  }

  virtual bool Accept(LogPointer log) {
    return (log->log_data().tv.tv_sec > time_ref_ );
  }

private:
  time_t time_ref_;
};

/*
 * Filter based on a generic pattern for all text-based fields of the log
 */

class LogFilterItemPattern : public LogFilterItem {
public:

  LogFilterItemPattern(const std::string& pattern)
    : simple_pattern_(pattern) {
  }

  virtual bool Accept(LogPointer log) {
    return ( log->Match(simple_pattern_));
  }

private:

  SimplePattern simple_pattern_;
};

/*
 * Filter based on a firect pattern matching of a generic field
 */

class LogFilterItemEqual : public LogFilterItem {
public:

  LogFilterItemEqual(const std::string&  field, const std::string&  values) {
    field_ = field;
    values_ = au::split(values, ',');   // Allow multiple values separated by commas
    // Create patterns for all values
    for (size_t i = 0; i < values_.size(); i++) {
      patterns_.push_back(new SimplePattern(values_[i]));
    }
  }

  virtual ~LogFilterItemEqual() {
    patterns_.clearVector();
  }

  virtual bool Accept(LogPointer log) {
    std::string value = log->Get(field_);

    for (size_t i = 0; i < patterns_.size(); i++) {
      if (patterns_[i]->match(value)) {
        return true;
      }
    }
    return false;
  }

private:

  std::string command_;
  std::string field_;
  std::vector<std::string> values_;
  au::vector<SimplePattern> patterns_;
};

class LogFilter {
public:

  static au::SharedPointer<LogFilter> Create(const std::string& definition, au::ErrorManager& error) {
    au::SharedPointer<LogFilter> log_filter(new LogFilter(definition));
    std::vector<std::string> commands = au::split(definition, '|');        // Split in commands with "|" separator
    for (size_t i = 0; i < commands.size(); i++) {
      au::SharedPointer<LogFilterItem> item = LogFilterItem::Create(commands[i], error);
      if (error.IsActivated()) {
        return au::SharedPointer<LogFilter>(NULL);
      }
      log_filter->items_.push_back(item);
    }

    return log_filter;
  }

  bool Accept(LogPointer log) {
    for (size_t i = 0; i < items_.size(); i++) {
      if (!items_[i]->Accept(log)) {
        return false;
      }
    }
    return true;
  }

private:

  LogFilter(const std::string& definition) {
    definition_ = definition;   // Keep definition for debugging
  }

  std::string definition_;
  std::vector< au::SharedPointer<LogFilterItem> > items_;
};


typedef au::SharedPointer<LogFilter>   LogFilterPointer;
}

#endif  // ifndef AU_LOG_FILTER_