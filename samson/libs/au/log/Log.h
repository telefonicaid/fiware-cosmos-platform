
#ifndef _H_AU_LOG
#define _H_AU_LOG


#define DATE_FORMAT "%A %d %h %H:%M:%S %Y"

#include <fcntl.h>
#include <regex.h>

#include "au/Status.h"
#include "au/TemporalBuffer.h"
#include "au/Tokenizer.h"
#include "au/containers/StringVector.h"
#include "au/containers/vector.h"
#include "au/network/FileDescriptor.h"
#include "au/string/split.h"
#include "au/tables/Table.h"

namespace au {
/*
 *
 * Known string fields
 * ---------------------------------------------------------------------------
 * "progName";
 * "text";
 * "file";
 * "fName";
 * "stre";
 *
 * Formats:
 * ---------------------------------------------------------------------------
 *
 * HOST     ( part of the logServer information )
 *
 * TYPE     ( in LogData )
 * PID      ( in LogData )
 * DATE     ( in LogData )
 * date     ( in LogData )
 * TIME     ( in LogData )
 * time     ( in LogData )
 * TID      ( in LogData )
 * LINE     ( in LogData )
 * TLEV     ( in LogData )
 * EXEC     progName     // Name of the executable ( progname )
 * AUX      aux          // aux variable to distinguish fork files...
 * FILE     file         // Name of the source file
 * TEXT     text         // Message
 * text     text         // Message limited to 60 chars
 * FUNC     fname        // Function name
 * STR      stre         // ?
 * time_unix
 */

extern const char *log_reseved_words[];

// Entry in the log
class Log {
public:

  Log() {
  }

  ~Log() {
  }

  // Main log data
  LogData& log_data();

  // Set and get methods
  void Set(const std::string& field_name, const std::string& field_value);
  std::string Get(const std::string& name, const std::string& default_value);
  std::string Get(std::string name);

  // Read and Write over a file descriptor ( network or disk )
  bool Read(au::FileDescriptor *fd);
  bool Write(au::FileDescriptor *fd);

  // Debug string
  std::string str();

  // Get total number og bytes when serialized
  size_t SerialitzationSize();

  // Match agains a particuar regular expression
  bool match(const regex_t *preg) {
    std::map<std::string, std::string>::iterator it_fields;
    for (it_fields = fields_.begin(); it_fields != fields_.end(); it_fields++) {
      std::string value = it_fields->second;
      int c = regexec(preg, value.c_str(), 0, NULL, 0);
      if (c == 0) {
        return true;
      }
    }
    return false;
  }

  // Spetial log to define mark of new session
  void SetNewSession();
  bool IsNewSession();

private:

  // Methods to serialize string-kind fields
  size_t getStringsSize();
  void copyStrings(char *data);
  void addStrings(char *strings, size_t len);

  LogData log_data_;
  std::map<std::string, std::string> fields_;
};
}

#endif  // ifndef _H_AU_LOG