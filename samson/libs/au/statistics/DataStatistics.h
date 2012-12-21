
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


#ifndef _H_AU_DATA_STATISTICS
#define _H_AU_DATA_STATISTICS

#include <math.h>
#include <time.h>

#include <cstring>
#include <list>
#include <memory>
#include <string>

#include "au/containers/map.h"
#include "au/mutex/Token.h"
#include "au/singleton/Singleton.h"
#include "au/statistics/Averager.h"
#include "au/statistics/Rate.h"
#include "au/string/StringUtilities.h"


namespace au {
/**
 * \brief Class to keep rate statistics about several concepts identified by a string
 *
 */
class RateStatistics {
public:

  // Private constructor since it is only used as singleton with au::Singleton
  ~RateStatistics() {
    rates_.clearMap();
  }

  /**
   * \brief Push a new value to the class
   */
  void Push(const std::string& concept, double value);
  /**
   * \brief Get rate in units/s ( this is an accumulated values per unit of time )
   */
  double GetRate(const std::string& concept) const;

  /**
   * \brief Get the total sum of pushed values
   */
  double GetTotal(const std::string& concept) const;

  /**
   * \brief Get a debug string about a rate & total of a concept
   */
  std::string GetRateAndTotalString(const std::string& concept, const std::string& unit) {
    return au::str(GetRate(concept), unit + "/s") + " | " + au::str(GetTotal(concept), unit);
  }

  /**
   * \brief Get a debug string about a rate of a concept
   */
  std::string GetRateString(const std::string& concept, const std::string& unit) {
    return au::str(GetRate(concept), unit + "/s");
  }

  /**
   * \brief Get all concepts registered so far
   */
  std::vector<std::string> GetConcepts() const {
    return rates_.getKeysVector();
  }

private:

  friend class Singleton<RateStatistics>;
  RateStatistics() : token_("RateStatistics") {
  }

  mutable Token token_;
  au::map<std::string, Rate> rates_;
};

/**
 * \brief Class to keep average of several concepts identified by a string
 *
 */
class AverageStatistics {
public:

  // Private constructor since it is only used as singleton with au::Singleton
  ~AverageStatistics() {
    averagers_.clearMap();
  }

  /**
   * \brief Push a new value to the class
   */
  void Push(const std::string& concept, double value);

  /**
   * \brief Get average of pushed values
   */
  double GetAverage(const std::string& concept, double default_value = 0) const;

  /**
   * \brief Get deviation in the average compuation
   */
  double GetAverageDeviation(const std::string& concept, double default_value = 0) const;

  /**
   * \brief Get a debug string about a average & deviation of a concept
   */
  std::string GetAverageAndDeviationString(const std::string& concept, const std::string& unit) {
    return au::str(GetAverage(concept), unit) + " ~ " + au::str(GetAverageDeviation(concept), unit);
  }

  /**
   * \brief Get a debug string about a averageof a concept
   */
  std::string GetAverageString(const std::string& concept, const std::string& unit) {
    return au::str(GetAverage(concept), unit);
  }

  /**
   * \brief Get all concepts registered so far
   */
  std::vector<std::string> GetConcepts() const {
    return averagers_.getKeysVector();
  }

private:

  friend class Singleton<AverageStatistics>;
  AverageStatistics() : token_("AverageStatistics") {
  }

  mutable Token token_;
  au::map<std::string, Averager> averagers_;
};
}
#endif  // ifndef _H_AU_DATA_STATISTICS
