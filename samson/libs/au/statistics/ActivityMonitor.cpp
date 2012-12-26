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


#include "ActivityMonitor.h"  // Own interface

#include <math.h>

#include "au/log/LogMain.h"

namespace au {
/**
 * \brief Namespace for au::statistics library (Handy classes for statistics)
 */
namespace statistics {
ActivityStatistics::ActivityStatistics(const std::string& name) {
  name_ = name;
  total_ = 0;
  total_square_ = 0;
  last_ = 0;
  min_ = 0;
  max_ = 0;
  counter_ = 0;
}

void ActivityStatistics::Push(double t) {
  if (time < 0) {
    LOG_SW(("Reported negative time (%d) for item %s", t, name_.c_str()));
    return;
  }

  ++counter_;     // Update the counter ( # of times executed )
  total_ += t;
  total_square_ += t * t;
  last_ = t;

  if (counter_ == 1) {
    min_ = t;
    max_ = t;
  } else {
    // Update min & max
    if (t < min_) {
      min_ = t;
    }
    if (t > max_) {
      max_ = t;
    }
  }
}

double ActivityStatistics::total() const {
  return total_;
}

double ActivityStatistics::last() const {
  return last_;
}

double ActivityStatistics::min() const {
  return min_;
}

double ActivityStatistics::max() const {
  return max_;
}

size_t ActivityStatistics::counter() const {
  return counter_;
}

double ActivityStatistics::GetAverage() const {
  if (counter_ == 0) {
    return 0;
  }
  return total_ / (double)counter_;
}

double ActivityStatistics::GetStdDeviation() const {
  if (counter_ == 0) {
    return 0;
  }

  double average = GetAverage();
  double tmp = (total_square_ / (double)counter_) - average * average;
  if (tmp < 0) {
    LOG_SW(("Error computing std deviation in ActivityStatistics"));
    return 0;
  }
  return sqrt(tmp);
}

ActivityMonitor::ActivityMonitor(const std::string& first_activity_name)
  : token_("EngineStatistics") {
  current_activty_ = first_activity_name;
  current_activirty_start_time_ = 0;
}

void ActivityMonitor::StartActivity(const std::string& activity_name) {
  au::TokenTaker tt(&token_);
  double stop_time = cronometer_.seconds();
  double time = stop_time - current_activirty_start_time_;

  // Inset in the list of last items
  items_.push_back(new ActivityItem(current_activty_, time));
  // Only keep the list of last 100 elements
  while (items_.size() > 100) {
    ActivityItem *tmp_item = items_.extractFront();
    delete tmp_item;
  }

  // Update the associated element statustics
  ActivityStatistics *activity_estatistics = elements_.findOrCreate(current_activty_, current_activty_);
  activity_estatistics->Push(time);

  // Change the name of the current activty
  current_activirty_start_time_ = stop_time;
  current_activty_ = activity_name;
}

void ActivityMonitor::StopActivity() {
  // Mutex protection inside start_activity
  StartActivity("no_activity");
}

std::string ActivityMonitor::GetLastItemsTable() const {
  au::TokenTaker tt(&token_);

  au::tables::Table table("Item|Time,left,f=double");

  au::list<ActivityItem>::const_iterator it;
  for (it = items_.begin(); it != items_.end(); ++it) {
    table.addRow(au::StringVector((*it)->name(), au::str("%.12f", (*it)->time()).c_str()));
  }
  return table.str();
}

double ActivityMonitor::GetTotalTimeForActivity(const std::string name) const {
  ActivityStatistics *activity = elements_.findInMap(name);

  if (!activity) {
    return 0;
  }
  return activity->total();
}

std::string ActivityMonitor::GetSummary() const {
  std::ostringstream output;

  au::TokenTaker tt(&token_);


  au::map<std::string, ActivityStatistics >::const_iterator it;
  for (it = elements_.begin(); it != elements_.end(); ++it) {
    output << "[" << it->first << ":" <<  au::str(it->second->total(), "s")  << "]";
  }
  return output.str();
}

std::string ActivityMonitor::GetElementsTable() const {
  au::TokenTaker tt(&token_);

  au::tables::Table table(
    "Element|Num,f=uint64|Total time,f=double|Average,f=double|std dev,f=double|Min,f=double|Max,f=double");

  table.setTitle("Engine statistics");

  au::map<std::string, ActivityStatistics >::const_iterator it;
  for (it = elements_.begin(); it != elements_.end(); ++it) {
    au::StringVector values;

    values.push_back(it->first);

    values.push_back(au::str("%lu", (size_t)(it->second->counter())));
    values.push_back(au::str("%.12f", it->second->total()));

    values.push_back(au::str("%.12f", it->second->GetAverage()));
    values.push_back(au::str("%.12f", it->second->GetStdDeviation()));

    values.push_back(au::str("%.12f", it->second->min()));
    values.push_back(au::str("%.12f", it->second->max()));

    table.addRow(values);
  }

  au::StringVector sort_fields = au::StringVector("Total time");
  table.sort(sort_fields);

  return table.str();
}

std::string ActivityMonitor::GetCurrentActivity() const {
  au::TokenTaker tt(&token_);

  return current_activty_;
}
}
}