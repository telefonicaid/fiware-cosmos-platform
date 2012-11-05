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

#include <math.h>

#include "OnOffMonitor.h"  // Own interface

namespace au {
OnOffMonitor::OnOffMonitor(double time_span) {
  on_ = false;       // By default it is initially false
  on_time_ = 0;
  off_time_ = 0;

  factor_ = ( time_span - 1 ) / time_span;
}

double OnOffMonitor::on_time() const {
  return on_time_;
}

double OnOffMonitor::off_time() const {
  return off_time_;
}

void OnOffMonitor::Set(bool on) {
  double t = cronometer.seconds();

  if (on_) {
    on_time_ += t;
  } else {
    off_time_ += t;  // Forgetting factor
  }
  off_time_ *= ::pow(factor_, t);
  on_time_ *= ::pow(factor_, t);

  // Reset cronometer
  cronometer.Reset();

  // Change state if any
  on_ = on;
}

double OnOffMonitor::activity_percentadge() const {
  double t = cronometer.seconds();
  double on_time = on_time_;
  double off_time = off_time_;

  if (on_) {
    on_time += t;
  } else {
    off_time += t;
  }
  if (( on_time + off_time ) == 0) {
    return 0;
  } else {
    return on_time / ( on_time + off_time );
  }
}
}