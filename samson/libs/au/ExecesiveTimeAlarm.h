
#ifndef AU_ExecesiveTimeAlarm_H_
#define AU_ExecesiveTimeAlarm_H_

#include "au/statistics/Cronometer.h"
#include <string>

namespace au {
class ExecesiveTimeAlarm {
public:

  ExecesiveTimeAlarm(const std::string& title);
  ExecesiveTimeAlarm(const std::string& title, double max_time);
  ~ExecesiveTimeAlarm();

private:

  Cronometer cronometer_;
  std::string title_;
  double max_time_;
};
}

#endif  // ifndef AU_ExecesiveTimeAlarm_H_