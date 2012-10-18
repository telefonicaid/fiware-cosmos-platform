

/* ****************************************************************************
*
* FILE            OnOffMonitor.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            2012
*
* DESCRIPTION
*
* Classes used to monitor and on-off system determining how much time it is in each state
*
* COPYRIGTH       Copyright 2012 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/


#ifndef _H_AU_ONOFFMONITOR
#define _H_AU_ONOFFMONITOR

#include "au/statistics/Cronometer.h"
#include "logMsg/logMsg.h"
#include <string>

namespace au {
// Class used to monitor a on-off system ( % time in each state )
class OnOffMonitor {
public:

  OnOffMonitor(double time_span = 30);
  ~OnOffMonitor() {
  }

  // Set state
  void Set(bool _on);

  // Get on %
  double activity_percentadge() const;

  // Get time for each state
  double on_time() const;
  double off_time() const;

private:

  au::Cronometer cronometer;   // Cronometer since the last change
  bool on_;                    // Flag to determine current position

  double on_time_;     // Accumulated time
  double off_time_;

  double factor_;       // Factor for the update
};
}

#endif  // ifndef _H_AU_ONOFFMONITOR