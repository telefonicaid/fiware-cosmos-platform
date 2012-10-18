
#ifndef _H_AU_LOG
#define _H_AU_LOG

#define DATE_FORMAT "%A %d %h %H:%M:%S %Y"

#include <fcntl.h>

#include "au/string/Pattern.h"
#include "au/Status.h"
#include "au/TemporalBuffer.h"
#include "au/string/Tokenizer.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/StringVector.h"
#include "au/containers/vector.h"
#include "au/log/LogCommon.h"
#include "au/network/FileDescriptor.h"
#include "au/string/split.h"
#include "au/tables/Table.h"
#include "au/tables/Table.h"

namespace au {
/*
 *
 * Known fields
 * ---------------------------------------------------------------------------
 *
 * channel        Number of channel emited ( example  0 )
 * channel_name   Name of channel emited ( example message )
 * channel_alias  Alias of channel emited ( example M )
 *
 * host        Name of the host ( if recovered using LogServer )
 *
 * pid         Number of process that generated this
 * tid         Identifier of the thread
 *
 * DATE        Date and time formats
 * date
 * TIME
 * time
 * timestamp
 * time_unix   Integuer version of the time
 *
 * line
 * exec        Name of the executable ( progname )
 * file        Name of the source file
 * text        Message
 * text80      Message ( limited to 80 chars )
 * function    Function name
 */

extern const char *log_reseved_words[];

struct LogData {
  int channel;            // Channel where this log was emitted
  int line;               // Line number
  struct timeval tv;      // time since 1970
  int timezone;           // The timezone
  pid_t pid;              // pid of the process
  pid_t tid;              // Identifier of the thread
};

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
  template< typename T>
  inline void Set(const std::string& field_name, const T& t) {
    std::ostringstream output;

    output << t;
    Set(field_name, output.str());
  }

  std::string Get(const std::string& name, const std::string& default_value) const;
  std::string Get(std::string name) const;

  // Read and Write over a file descriptor ( network or disk )
  bool Read(au::FileDescriptor *fd);
  bool Write(au::FileDescriptor *fd);

  // Debug string
  std::string str() const;

  // Get total number og bytes when serialized
  size_t SerialitzationSize();

  // Match agains a particuar regular expression
  bool match(Pattern *pattern) const;
  int channel() const;
  // Spetial log to define mark of new session
  void SetNewSession();
  bool IsNewSession() const;

private:

  // Methods to serialize string-kind fields
  size_t getStringsSize() const;
  void copyStrings(char *data);
  void addStrings(char *strings, size_t len);

  LogData log_data_;
  std::map<std::string, std::string> fields_;
};

typedef au::SharedPointer<Log>   LogPointer;

// Table of fields
au::SharedPointer<au::tables::Table> getTableOfFields();
}

#endif  // ifndef _H_AU_LOG