
#ifndef CONCEPT_TIME_AU_H_
#define CONCEPT_TIME_AU_H_

#include <string>

#include "au/containers/map.h"
#include "au/statistics/Cronometer.h"

namespace au {
class ConceptTimeCounterItem {
public:
  ConceptTimeCounterItem() {
  }

  ~ConceptTimeCounterItem() {
  }

  double GetTime() const {
    return cronometer_.seconds();
  }

  void ResetCronometer() {
    cronometer_.Reset();
  }

private:

  au::Cronometer cronometer_;
};

class ConceptTimeCounter {
public:

  ConceptTimeCounter(double max_time = 60) {
    max_time_ =  max_time;
  }

  /**
   * \brief Add a concept
   * This add counter or reset cronometer for this concept
   */
  void Add(const std::string& concept) {
    ConceptTimeCounterItem *item =  items_.findInMap(concept);

    if (item) {
      item->ResetCronometer();
    }
    items_.insertInMap(concept, new ConceptTimeCounterItem());
  }

  /**
   * \brief Get time for a paricular concept or -1 if it does not exist
   */

  double GetTime(const std::string& concept) const {
    ConceptTimeCounterItem *item =  items_.findInMap(concept);

    if (item) {
      double time = item->GetTime();
      if (time > max_time_) {
        return -1;   // Will be reoved in next review
      }
      return time;
    }
    return -1;   // It does not exist
  }

  /**
   * \brief Check if a name is active
   */

  bool IsActive(const std::string& concept) const {
    double time = GetTime(concept);

    return (time != -1);
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