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

#ifndef _H_AU_LOG
#define _H_AU_LOG

#define DATE_FORMAT "%A %d %h %H:%M:%S %Y"

#include <fcntl.h>

#include "au/string/Pattern.h"
#include "au/string/StringUtilities.h"
#include "au/Status.h"
#include "au/TemporalBuffer.h"
#include "au/string/Tokenizer.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/StringVector.h"
#include "au/containers/vector.h"
#include "au/log/LogCommon.h"
#include "au/network/FileDescriptor.h"
#include "au/string/StringUtilities.h"
#include "au/tables/Table.h"
#include "au/tables/Table.h"

namespace au {
/*
 *
 * ---------------------------------------------------------------------------
 * Fields of every log
 * ---------------------------------------------------------------------------
 *
 * type        One letter describing severity of the log ( D,M,V,W,E,X )
 * channel     Name of channel emited ( example message )
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
 * file        Name of the source file
 * line        Line inside the source file
 * function    Function name
 * exec        Name of the executable ( progname )
 * text        Message
 * text80      Message ( limited to 80 chars )
 */

extern const char *log_reseved_words[];

struct LogData {
  char level;             // Level of the channel ( D , M , W , E .... )
  int channel;            // Channel where this log was emitted
  int line;               // Line number
  struct timeval tv;      // time since 1970
  int timezone;           // The timezone
  pid_t pid;              // pid of the process
  size_t tid;             // Identifier of the thread ( opaque type over a size_t type )
};

// Entry in the log
class Log {
  
public:

  Log() {}
  ~Log() {}

  // Set methods
  void Set(const std::string& field_name, const std::string& field_value);
  template< typename T>
  inline void Set(const std::string& field_name, const T& t) {
    std::ostringstream output;
    output << t;
    Set(field_name, output.str());
  }

  // Get methods
  std::string Get(const std::string& name, const std::string& default_value) const;
  std::string Get(std::string name) const;
  LogData& log_data();
  int channel() const;
  int level() const;

  // Read and Write over a file descriptor ( network or disk )
  bool Read(au::FileDescriptor *fd);
  bool Write(au::FileDescriptor *fd);

  // Debug string
  std::string str() const;

  // Get total number og bytes when serialized
  size_t SerialitzationSize();

  // Match agains a particuar regular expression
  bool Match(Pattern& pattern) const;
  bool Match(SimplePattern& pattern) const;
  
  // Spetial log to define mark of new session
  void SetNewSession();
  bool IsNewSession() const;
  
  // Fancy functions to get the color of this log on screen
  au::Color GetColor();

  // transform level in letter
  static int GetLogLevel( const std::string& str_log_level );
  static std::string GetLogLevel( int log_level );
  
private:

  // Methods to serialize string-kind fields
  size_t GetStringsSize() const;
  void CopyStrings(char *data);
  void AddStrings(char *strings, size_t len);

  LogData log_data_;
  std::map<std::string, std::string> fields_;
};

  std::vector< au::SharedPointer<Log> > readLogFile(std::string file_name, au::ErrorManager& error);
au::SharedPointer<au::tables::Table> getTableOfFields();
typedef au::SharedPointer<Log>   LogPointer;

}

#endif  // ifndef _H_AU_LOG