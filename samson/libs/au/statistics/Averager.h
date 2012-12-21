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
* FILE            Averager
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            December 2012
*
* DESCRIPTION
*
*  Class used to compute the average of an unbounded sequence of values
*
* ****************************************************************************/


#ifndef _H_AU_AVERAGER
#define _H_AU_AVERAGER

#include <math.h>
#include <time.h>

#include <cstring>
#include <list>
#include <memory>
#include <string>

#include "au/statistics/Cronometer.h"
#include "au/statistics/Cronometer.h"
#include "au/string/StringUtilities.h"

namespace au {
/**
 * \brief Average : Class to compute average and std-deviation of an unbounded streams of values
 */

class Averager {
public:

  Averager() : mean_(0), mean_square_(0) {
  }

  /**
   * \brief Add a new value to take into account
   */
  void Push(double value);

  /**
   * \brief Get teh average computed so far
   */
  double GetAverage()  const;

  /**
   * \brief Get std-deviation computed so far
   */
  double GetDeviation() const;

  /**
   * \brief string verion of average + deviation
   */

  std::string str(const std::string& unit = "") const;

  /**
   * \brief string verion of average
   */

  std::string str_mean(const std::string& unit = "") const;


  /**
   * \brief Get string containing the last observed values
   */
  std::string GetLastValues() const;

private:

  double mean_;
  double mean_square_;

  std::list<std::string> last_values_;
};
}                     // end of namespace au::rate

#endif  // ifndef _H_AU_RATE
