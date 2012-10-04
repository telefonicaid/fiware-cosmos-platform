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

/*
 * FILE            TableLogFormatter
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 */

#include "./TableLogFormatter.h"  // Own interface
#include "au/log/Log.h"
#include "au/log/LogFormatter.h"
#include <string>               // std::string

#define Char_to_int(x) ((x) - 48)

/* Map timelocal() to mktime() since the latter is compatible with the former,
 * which does not exist on Solaris.
 */
#if defined(__sun__)
#define timelocal(x)   mktime(x)
#endif  /* __sun__ */

namespace au {
TableLogFormatter::TableLogFormatter(std::string _format) {
  // Keep format for fields
  format = _format;

  // Default values
  is_table = false;
  is_reverse = false;
  limit = 0;                   // No max number of records by default
  num_logs = 0;

  // init function called
  flag_init = false;

  // No filter by time by default
  ref_time = 0;

  // Init new session flag
  flag_new_session_found = false;

  table = NULL;
  log_formatter = NULL;
  pattern = NULL;
}

TableLogFormatter::~TableLogFormatter() {
  if (table) {
    delete table;
  }
  if (log_formatter) {
    delete log_formatter;
  }
}

void TableLogFormatter::set_as_table(bool _is_table) {
  if (flag_init) {
    LM_W(("Setup methods in TableLogFormatter can only be called during init phase."));
    return;
  }

  is_table = _is_table;
}

void TableLogFormatter::set_channel(std::string _channel) {
  channel = _channel;
}

void TableLogFormatter::set_reverse(bool _is_reverse) {
  if (flag_init) {
    LM_W(("Setup methods in TableLogFormatter can only be called during init phase."));
    return;
  }

  is_reverse =  _is_reverse;
}

void TableLogFormatter::set_as_multi_session(bool _is_multi_session) {
  if (flag_init) {
    LM_W(("Setup methods in TableLogFormatter can only be called during init phase."));
    return;
  }

  is_multi_session =  _is_multi_session;
}

void TableLogFormatter::set_limit(int _limit) {
  if (flag_init) {
    LM_W(("Setup methods in TableLogFormatter can only be called during init phase."));
    return;
  }

  if (_limit < 0) {
    limit = 0;
  }
  limit = _limit;
}

void TableLogFormatter::set_pattern(std::string _str_pattern) {
  if (flag_init) {
    LM_W(("Setup methods in TableLogFormatter can only be called during init phase."));
    return;
  }

  str_pattern = _str_pattern;
}

void TableLogFormatter::set_time(std::string _str_time) {
  if (flag_init) {
    LM_W(("Setup methods in TableLogFormatter can only be called during init phase."));
    return;
  }

  str_time = _str_time;
}

void TableLogFormatter::set_date(std::string _str_date) {
  if (flag_init) {
    LM_W(("Setup methods in TableLogFormatter can only be called during init phase."));
    return;
  }

  str_date = _str_date;
}

void TableLogFormatter::set_type(std::string _str_type) {
  if (flag_init) {
    LM_W(("Setup methods in TableLogFormatter can only be called during init phase."));
    return;
  }

  str_type = _str_type;
}

void TableLogFormatter::init(ErrorManager& error) {
  // Make sure we only call this method once
  if (flag_init) {
    LM_W(("init called twice in TableLogFormatter"));
    error.set("Init called twice");
    return;
  }
  flag_init = true;

  if (is_table) {
    // Table fields
    table_fields = split_using_multiple_separators(format, "|:;,-/\[] ().");

    std::string table_definition;
    for (size_t i = 0; i < table_fields.size(); i++) {
      if (table_fields[i].length() > 0) {
        table_definition.append(au::str("%s,left|", table_fields[i].c_str()));
      }
    }

    table = new au::tables::Table(table_definition);
    log_formatter = NULL;
  } else {
    table = NULL;
    log_formatter = new LogFormatter(format);
  }


  // Patern restriction
  if (str_pattern != "") {
    pattern = new Pattern(str_pattern, error);
    if (error.IsActivated()) {
      return;
    }
  }

  // Time restrictions
  if (( str_time != "" ) || ( str_date != "" )) {
    time_t now = time(NULL);
    struct tm tm;
    gmtime_r(&now, &tm);

    if (str_time != "") {
      if (str_time.length() != 8) {
        error.set("Error: Wrong format for -time. It is -time HH:MM:SS\n");
        return;
      }

      tm.tm_hour = Char_to_int(str_time[0]) * 10 + Char_to_int(str_time[1]);
      tm.tm_min = Char_to_int(str_time[3]) * 10 + Char_to_int(str_time[4]);
      tm.tm_sec = Char_to_int(str_time[6]) * 10 + Char_to_int(str_time[7]);

      LM_V(("Time interpretado %d:%d:%d", tm.tm_hour, tm.tm_min, tm.tm_sec ));
    }

    if (str_date != "") {
      if (str_date.length() != 8) {
        error.set("Error: Wrong format for -date. It is -date DD/MM/YY\n");
      }

      // DD/MM/YY
      tm.tm_year = 100 + Char_to_int(str_date[6]) * 10 + Char_to_int(str_date[7]);
      tm.tm_mon  = Char_to_int(str_date[3]) * 10 + Char_to_int(str_date[4]) - 1;
      tm.tm_mday = Char_to_int(str_date[0]) * 10 + Char_to_int(str_date[1]);
    }

    // Create a different time
    ref_time = timelocal(&tm);
  }
}

bool TableLogFormatter::filter(au::SharedPointer<Log> log) {
  if (!flag_init) {
    LM_W(("Not possible to filter logs if init has not been called"));
    return false;
  }

  // Filter by number of records
  if (limit > 0) {
    if (num_logs >= limit) {
      return false;
    }
  }

  // Filter by pattern
  if (pattern) {
    if (!log->match(pattern)) {
      return false;
    }
  }

  // filter by time
  if (ref_time > 0) {
    if (!(log->log_data().tv.tv_sec <= ref_time)) {
      return false;
    }
  }
/*
  // Check if the type is correct
  if (str_type != "") {
    if (log->log_data().type != str_type[0]) {
      return false;
    }
  }
 */

  if (channel != "") {
    if (log->Get("channel") != channel) {
      return false;
    }
  }

  return true;
}

void TableLogFormatter::add(au::SharedPointer<Log> log) {
  if (!flag_init) {
    LM_W(("Not possible to add TableLogFormatter if not init"));
    return;
  }

  // Filter logs by different criteria
  if (!filter(log)) {
    return;
  }

  // Detect a new session mark...
  if (flag_new_session_found) {
    // Not add more records if a new session mark has been previously found
    return;
  }

  if (!is_multi_session) {
    if (log->IsNewSession()) {
      flag_new_session_found = true;
      return;
    }
  }
  // Add the counter of logs
  num_logs++;

  if (is_table) {
    StringVector values;
    for (size_t i = 0; i < table_fields.size(); i++) {
      values.push_back(log->Get(table_fields[i]));
    }
    table->addRow(values);
  } else {
    std::string txt = log_formatter->get(log);
    LM_V(("%s // %s", txt.c_str(), log->str().c_str()));
    output << txt << "\n";
  }
}

std::string TableLogFormatter::str() {
  if (table) {
    if (is_reverse) {
      table->reverseRows();
    }
    return table->str();
  } else {
    std::string lines = output.str();
    if (is_reverse) {
      return au::reverse_lines(lines);
    } else {
      return lines;
    }
  }
}
}
