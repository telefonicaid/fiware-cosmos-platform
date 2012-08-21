

#ifndef _H_AU_LOG_FILE
#define _H_AU_LOG_FILE

#include "au/containers/vector.h"

namespace au {
class Log;

class LogFile {
public:

  // Collections of logs from this file
  au::vector<Log> logs;

  // Read a file
  static au::Status read(std::string file_name, LogFile **logFile);
};
}

#endif
