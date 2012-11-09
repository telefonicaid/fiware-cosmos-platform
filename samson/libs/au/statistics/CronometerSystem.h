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


#ifndef AU_CRONOMETER_SYSTEM_H_
#define AU_CRONOMETER_SYSTEM_H_

#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/statistics/Cronometer.h"

namespace au {
class CronometerSystem {
public:

  CronometerSystem();
  ~CronometerSystem() {
  }

  // Start stop methods
  void Start();
  void Stop();

  // Reset the count to 0
  void Reset();

  // Get information
  double running_seconds() const;
  double seconds() const;

  // Get time informtion formatted correctly
  std::string str_seconds() const;
  
private:

  Cronometer cronometer_;
  int seconds_;
  bool running_;

  friend std::ostream& operator<<(std::ostream&, const CronometerSystem& cronometer_system);
};
}

#endif  // ifndef AU_CRONOMETER_SYSTEM_H_