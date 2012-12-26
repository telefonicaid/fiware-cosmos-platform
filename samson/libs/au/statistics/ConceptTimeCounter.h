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

#ifndef CONCEPT_TIME_AU_H_
#define CONCEPT_TIME_AU_H_

#include <string>

#include "au/containers/map.h"
#include "au/statistics/Cronometer.h"

namespace au {
/**
 * \brief Item inside ConceptTimeCounter class
 */

class ConceptTimeCounterItem {
public:
  ConceptTimeCounterItem() {
  }

  ~ConceptTimeCounterItem() {
  }

  /**
   * \brief Get time since creation of this item or last reset
   */
  double GetTime() const {
    return cronometer_.seconds();
  }

  /**
   * \brief Reset internal cronometer
   */
  void ResetCronometer() {
    cronometer_.Reset();
  }

private:
  au::Cronometer cronometer_;
};

class ConceptTimeCounter {
public:
  /**
   * \brief Constructor
   * \param max_time Max time to consider an item "active" in seconds
   */
  explicit ConceptTimeCounter(double max_time) {
    max_time_ =  max_time;
  }

  /**
   * \brief Destructor to free internal items map
   */
  ~ConceptTimeCounter() {
    items_.clearMap();
  }

  /**
   * \brief Add or Reset cronometer for a concept
   * This method adds a counter and/or resets its chronometer for the concept 'concept'.
   */
  void Add(const std::string& concept) {
    ConceptTimeCounterItem *item =  items_.findInMap(concept);

    if (item) {
      item->ResetCronometer();
    } else {
      items_.insertInMap(concept, new ConceptTimeCounterItem());
    }
  }

  /**
   * \brief Get time for a particular concept or -1 if it does not exist
   */

  double GetTime(const std::string& concept) const {
    ConceptTimeCounterItem *item =  items_.findInMap(concept);

    if (item) {
      double time = item->GetTime();
      if (time > max_time_) {
        return -1;   // Expired item will be removed ( from items_ ) in next Review call
      }
      return time;
    }
    return -1;   // It does not exist
  }

  /**
   * \brief Check whether 'concept' is active
   */

  bool IsActive(const std::string& concept) const {
    double time = GetTime(concept);

    return (time >= 0);
  }

  /**
   * \brief Review elements to remove old ones
   */

  void Review() {
    au::map< std::string, ConceptTimeCounterItem >::iterator iter;
    for (iter = items_.begin(); iter != items_.end(); ) {
      if (iter->second->GetTime() > max_time_) {
        items_.erase(iter++);
      } else {
        ++iter;
      }
    }
  }

  double time() const {
    return cronometer_.seconds();
  }

private:
  au::map< std::string, ConceptTimeCounterItem > items_;
  double max_time_;
  au::Cronometer cronometer_;
};
}

#endif  // #ifndef CONCEPT_TIME_AU_H_