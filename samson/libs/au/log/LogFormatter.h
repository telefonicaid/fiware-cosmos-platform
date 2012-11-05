

#ifndef _H_AU_LOG_FORMATTER
#define _H_AU_LOG_FORMATTER

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"
#include "au/log/LogCommon.h"

/*
 Class to transform a log into a string to displau somewhere with a particular format
 */

namespace au {
class LogFormatter {
public:

  // Constructor with initial format
  LogFormatter(const std::string& definition = AU_LOG_DEFAULT_FORMAT , bool color = false );

  // Modify the format used here
  void SetFormat(const std::string& definition , bool color = false );

  std::string definition() const {
    return definition_;
  }

  std::string get(au::SharedPointer<Log> log) const;
  
private:

  // Parse a log to get a string
  std::string GetIntern(au::SharedPointer<Log> log) const;

  std::string definition_;
  std::vector<std::string> fields_;
  bool color_; // Active colored output
};
}

#endif  // ifndef _H_AU_LOG_FORMATTER