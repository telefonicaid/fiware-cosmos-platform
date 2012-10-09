

#ifndef _H_AU_LOG_FORMATTER
#define _H_AU_LOG_FORMATTER

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"
#include "au/log/LogCommon.h"

namespace au {
class LogFormatter {
public:

  // Constructor with initial format
  LogFormatter(const std::string& definition = AU_LOG_DEFAULT_FORMAT);

  // Modify the format used here
  void set_format(const std::string& definition);

  // Parse a log to get a string
  std::string get(au::SharedPointer<Log> log) const;

  std::string definition() const {
    return definition_;
  }

private:
  std::string definition_;
  std::vector<std::string> fields_;
};
}

#endif  // ifndef _H_AU_LOG_FORMATTER