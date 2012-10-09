
#ifndef _H_AU_LOG_COUNTER
#define _H_AU_LOG_COUNTER

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"
#include "au/log/LogChannelFilter.h"
#include "au/log/LogFormatter.h"

namespace au {
class LogCounter {
public:

  void Process(LogPointer log) {
    size_t size = log->SerialitzationSize();

    rate_.Push(size);   // Global rate counter

    au::rate::Rate *rate = rates.findOrCreate(log->channel());
    rate->Push(size);   // Individual rate counter
  }

  std::string str(int c) {
    au::rate::Rate *rate = rates.findInMap(c);

    if (!rate) {
      return "";
    } else {
      return au::str("%s %s"
                     , au::str(rate->hits()).c_str()
                     , au::str(rate->size(), "B").c_str()
                     );
    }
  }

  std::string str_rate(int c) {
    au::rate::Rate *rate = rates.findInMap(c);

    if (!rate) {
      return "";
    } else {
      return au::str("%s", au::str(rate->hits()).c_str());
    }
  }

  size_t logs() {
    return rate_.hits();
  }

  size_t size() {
    return rate_.size();
  }

  size_t rate() {
    return rate_.rate();
  }

private:

  au::map<int, au::rate::Rate > rates;
  au::rate::Rate rate_;   // total rate
};
}

#endif // ifndef _H_AU_LOG_COUNTER