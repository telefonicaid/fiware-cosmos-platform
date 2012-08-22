

#ifndef _H_AU_LOG_FORMATTER
#define _H_AU_LOG_FORMATTER

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"

#define DEFAULT_FORMAT "TYPE : date : time : EXEC : FILE[LINE] : FUNC : TEXT"

namespace au {
class LogFormatter {
public:

  LogFormatter(const std::string& _definition = DEFAULT_FORMAT);
  std::string get(au::SharedPointer<Log> log) const;

private:
  std::string definition_;
  std::vector<std::string> fields_;
};
}

#endif  // ifndef _H_AU_LOG_FORMATTER