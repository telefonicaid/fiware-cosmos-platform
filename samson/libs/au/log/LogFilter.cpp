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


#include "LogFilter.h"  // Own interface

#include <time.h>


#define Char_to_int(x) ((x) - 48)

namespace au {
size_t GetTime(const std::string str_time) {
  time_t now = time(NULL);
  struct tm tm;

  gmtime_r(&now, &tm);

  tm.tm_hour = Char_to_int(str_time[0]) * 10 + Char_to_int(str_time[1]);
  tm.tm_min = Char_to_int(str_time[3]) * 10 + Char_to_int(str_time[4]);
  tm.tm_sec = Char_to_int(str_time[6]) * 10 + Char_to_int(str_time[7]);

  return timelocal(&tm);
}

size_t GetDate(const std::string& str_date) {
  time_t now = time(NULL);
  struct tm tm;

  gmtime_r(&now, &tm);

  tm.tm_year = 100 + Char_to_int(str_date[6]) * 10 + Char_to_int(str_date[7]);
  tm.tm_mon  = Char_to_int(str_date[3]) * 10 + Char_to_int(str_date[4]) - 1;
  tm.tm_mday = Char_to_int(str_date[0]) * 10 + Char_to_int(str_date[1]);

  return timelocal(&tm);
}

au::SharedPointer<LogFilterItem> LogFilterItem::Create(const std::string& command, au::ErrorManager& error) {
  if (command.length() == 0) {
    error.AddError("No command provided");
    return au::SharedPointer<LogFilterItem>(NULL);
  }

  // ~text --> pattern
  if (command[0] == '~') {
    return au::SharedPointer<LogFilterItem>(new LogFilterItemPattern(command.substr(1)));
  }

  // time>HH:mm

  if (au::CheckIfStringsBeginWith(command, "time>")) {
    if (command.length() != 10) {
      error.AddError("Expected format is time>HH::mm");
      return au::SharedPointer<LogFilterItem>(NULL);
    }

    return au::SharedPointer<LogFilterItem>(new LogFilterItemTimeRef(GetTime(command.substr(5))));
  }

  // date>DD:MM:YY

  if (au::CheckIfStringsBeginWith(command, "date>")) {
    if (command.length() != 13) {
      error.AddError("Expected format is date>DD:MM:YY");
      return au::SharedPointer<LogFilterItem>(NULL);
    }

    return au::SharedPointer<LogFilterItem>(new LogFilterItemTimeRef(GetDate(command.substr(5))));
  }

  // limit=XX

  if (au::CheckIfStringsBeginWith(command, "limit=")) {
    size_t limit = atoll(command.substr(6).c_str());
    if (limit == 0) {
      error.AddError("Wrong limit");
      return au::SharedPointer<LogFilterItem>(NULL);
    }
    return au::SharedPointer<LogFilterItem>(new LogFilterItemTimeLimit(limit));
  }

  // XX=XX
  size_t pos = command.find("=");
  if (pos == std::string::npos) {
    error.AddError(au::str("Error processing %s", command.c_str()));
    return au::SharedPointer<LogFilterItem>(NULL);
  }
  std::string field = command.substr(0, pos);
  std::string values = command.substr(pos + 1);
  LogFilterItemEqual *log_filter_item_equal = new LogFilterItemEqual(field, values);
  return au::SharedPointer<LogFilterItem>(log_filter_item_equal);
}
}