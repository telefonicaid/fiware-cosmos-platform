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

/* ****************************************************************************
*
* FILE            Cronometer
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Easy to use class to count how much seconds something happened.
*      Used in situations where time-precision is not required ( seconds are ok )
*
* ****************************************************************************/

#ifndef AU_CRONOMETER_H
#define AU_CRONOMETER_H

#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"


namespace au {
class Cronometer {
public:

  // Constructor & Destructor
  Cronometer();
  ~Cronometer() {
  }

  // Reset the count to 0
  void Reset();

  // Get the time difference since the last reset ( or creation ) in second
  double seconds() const;

  // Manual offset of the time
  void AddOffset(int offset);

  // String vecrsion of this
  std::string str();

  friend std::ostream& operator<<(std::ostream&, const Cronometer& cronometer);

  struct timeval t_;
};
}

#endif  // ifndef AU_CRONOMETER_H
