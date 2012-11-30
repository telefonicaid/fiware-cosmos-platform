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


#include "ExecesiveTimeAlarm.h"  // Own interface

#include "au/Log.h"
#include "au/log/LogMain.h"
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
    LM_T(logs.excessive_time, ("Excessive time ( %.4f > %.4f secs ) for '%s' "
                               , t
                               , max_time_
                               , title_.c_str()
                               ));
  }
}
}