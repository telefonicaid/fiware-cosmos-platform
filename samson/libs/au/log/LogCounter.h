
#ifndef _H_AU_LOG_COUNTER
#define _H_AU_LOG_COUNTER

#include "au/statistics/Rate.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/map.h"
#include "au/log/Log.h"
#include "au/log/LogChannelFilter.h"
#include "au/log/LogFormatter.h"

namespace au {
class LogCounter {
public:

  void Process(LogPointer log);

  // Accessors
  std::string str(int c) const;
  std::string str_rate(int c) const;
  size_t logs() const;
  size_t size() const;
  size_t rate() const;

private:

  au::map<int, au::rate::Rate > rates;
  au::rate::Rate rate_;   // total rate
};
}

#endif  // ifndef _H_AU_LOG_COUNTER