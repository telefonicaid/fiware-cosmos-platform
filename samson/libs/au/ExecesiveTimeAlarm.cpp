

#include "ExecesiveTimeAlarm.h"  // Own interface
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

namespace au {
ExecesiveTimeAlarm::ExecesiveTimeAlarm(const std::string& title) {
  title_ = title;
  max_time_ = 0.5;    // Default value
}

ExecesiveTimeAlarm::ExecesiveTimeAlarm(const std::string& title, double max_time) {
  title_ = title;
  max_time_ = max_time;
}

ExecesiveTimeAlarm::~ExecesiveTimeAlarm() {
  double t = cronometer_.seconds();

  if (t > max_time_) {
    LM_T(LmtExcesiveTime, ("Excessive time ( %.4f > %.4f secs ) for '%s' "
                           , t
                           , max_time_
                           , title_.c_str()
                           ));
  }
}
}