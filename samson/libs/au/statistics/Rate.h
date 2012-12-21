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
* FILE            Rate
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*  Class used to measure the bandwith of a particular operation
*  It automatically count the rate in the last minute and last hour
*
* ****************************************************************************/


#ifndef _H_AU_RATE
#define _H_AU_RATE

#include <math.h>
#include <time.h>

#include <cstring>
#include <list>
#include <memory>
#include <string>

#include "au/statistics/Cronometer.h"
#include "au/statistics/Cronometer.h"
#include "au/string/StringUtilities.h"


#include "au/mutex/Token.h"


namespace au {
class Rate {
public:

  // Constructor
  Rate(int num_seconds_to_average = 60);
  ~Rate();

  // Push new samples
  void Push(size_t size);

  // Clear data
  void Clear();

  // Get totals
  size_t hits() const;
  size_t size() const;

  // Get rates
  double hit_rate() const;
  double rate() const;

  // String to visualize this rate
  std::string str() const;

  /**
   * \brief Debug string with all internal accumulated values
   */
  std::string str_debug() const;

private:

  void UpdateTime() const;

  mutable au::Token token_;   // Mutex protection

  int num_samples_;   // Number of samples in hits_ and size_ vectors

  size_t total_size_;  // Total number of bytes
  size_t total_num_;  // Total number of hits

  int *hits_;         // Number of hits accumulated in the last "N" seconds
  double *size_;      // Total size accumulated in the last "N" seconds

  au::Cronometer c;
  mutable size_t last_time_correction;
};
}  // end of namespace au::rate

#endif  // ifndef _H_AU_RATE
