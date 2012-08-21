

#ifndef _H_AU_ACTIVITY_MONITOR
#define _H_AU_ACTIVITY_MONITOR

#include "au/Cronometer.h"
#include "au/Object.h"

#include "au/containers/map.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/tables/Table.h"

namespace au {
namespace statistics {
/*
 * ActivityMonitor is an agrgator of activity
 * Using simply methods StartActivity and StopActivity, ActivityMonitor compute
 * time spent in all activities and show agregated and averages for all of them
 *
 */


class ActivityItem : public au::Object {
public:

  ActivityItem(std::string name, double time) {
    name_ = name;
    time_ = time;
  }

  std::string name() const {
    return name_;
  }

  double time() const {
    return time_;
  }

private:

  std::string name_;       // Name of the item
  double time_;            // Time to be executed
};

class ActivityStatistics {
public:

  ActivityStatistics();

  void Push(ActivityItem *item);
  double GetAverage() const;
  double GetStdDeviation() const;

  double total()const ;
  double last()const ;
  double min()const ;
  double max()const ;
  size_t counter()const ;

private:

  double total_;            // Total time
  double total_square_;     // Total time*time
  double last_;             // Time of the last sample
  double min_;              // Min time
  double max_;              // Max time
  size_t counter_;          // Number of times
};

class ActivityMonitor {
public:

  ActivityMonitor(std::string first_activity_name = "init_activity");
  ~ActivityMonitor() {
  };

  // Start and Stop activities
  void StartActivity(std::string activity_name);
  void StopActivity();
  std::string GetCurrentActivity()const ;

  // Information for activities
  std::string GetLastItemsTable()const ;
  std::string GetElementsTable()const ;

private:

  au::Cronometer cronometer_;                 // Global cronometer
  std::string current_activty_;               // String describing the current activity
  double current_activirty_start_time_;       // Check point when the current task started

  au::list<ActivityItem> items_;     // Last items
  au::map<std::string, ActivityStatistics > elements_;

  mutable au::Token token;
};
}
}

#endif // ifndef _H_AU_ACTIVITY_MONITOR
