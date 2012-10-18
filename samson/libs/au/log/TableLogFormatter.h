#ifndef _H_AU_TABLE_LOG_FORATTER
#define _H_AU_TABLE_LOG_FORATTER

#include <fcntl.h>
#include <regex.h>
#include <regex.h>
#include <string>
#include <vector>

#include "au/string/Pattern.h"
#include "au/containers/SharedPointer.h"
#include "au/tables/Table.h"

namespace au {
class LogFormatter;
class Log;

// ---------------------------------------------------------
// TableLogFormatter
//
// Class used to generate tables of logs with different formats
// and filter criteria
//
// ---------------------------------------------------------

class TableLogFormatter {
  // Format string
  std::string format;

  // Table formatted
  std::vector<std::string> table_fields;
  au::tables::Table *table;

  // Formatter for logs
  LogFormatter *log_formatter;
  std::ostringstream output;

  // Pattern comparison
  Pattern *pattern;

  // Filter to only this Channel
  std::string channel;

  // Reference time to filter logs
  time_t ref_time;

  // Falg to indicate that a table-formatted output is required
  bool is_table;

  // Flag to indicate that reverse order is desiged
  bool is_reverse;

  // Set as multi session table
  bool is_multi_session;

  // Max number of records to be displayed
  int limit;
  int num_logs;

  // Pattern to filter records
  std::string str_pattern;
  std::string str_time;
  std::string str_date;
  std::string str_type;

  bool flag_init;     // Flag to indicate that init has been called

  bool flag_new_session_found;     // Flag to indicate that a new session mark has been found ( in no multi-session )

public:

  TableLogFormatter(std::string format);
  ~TableLogFormatter();

  // Init table log formatter after all setup is done ( call this only once )
  void init(ErrorManager& error);

  // Filter if this log should be added to the current table
  bool filter(au::SharedPointer<Log> log);

  // Flags to mofidy behavious
  void set_as_table(bool _is_table);
  void set_reverse(bool _is_reverse);
  void set_as_multi_session(bool _is_multi_session);
  void set_limit(int _limit);
  void set_pattern(std::string _str_pattern);
  void set_time(std::string _str_time);
  void set_date(std::string _str_date);
  void set_type(std::string _str_type);
  void set_channel(std::string channel);

  // Push a new log into the table to be print
  void add(au::SharedPointer<Log>);

  // Function to check if have enougth records ( not add anymore if return true )
  bool enougthRecords() {
    // In no-multi-session, this flags means that the new-session-mark was found
    if (flag_new_session_found) {
      return true;
    }

    // Check by the limit of recors
    if (limit == 0) {
      return false;
    }

    return (num_logs >= limit);
  }

  // Get the table to be print
  std::string str();
};
}

#endif  // ifndef _H_AU_TABLE_LOG_FORATTER
