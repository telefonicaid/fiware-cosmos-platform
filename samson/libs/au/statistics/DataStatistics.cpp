
#include "DataStatistics.h"  // Own interface

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

namespace au {
void DataStatistics::Push(const std::string& concept, double value) {
  TokenTaker tt(&token_);

  rates_.findOrCreate(concept)->Push(value);
  averagers_.findOrCreate(concept)->Push(value);
}

double DataStatistics::GetRate(const std::string& concept) const {
  TokenTaker tt(&token_);
  Rate *rate = rates_.findInMap(concept);

  if (rate != NULL) {
    return rate->rate();
  }
  return 0;
}

double DataStatistics::GetTotal(const std::string& concept) const {
  TokenTaker tt(&token_);
  Rate *rate = rates_.findInMap(concept);

  if (rate != NULL) {
    return rate->size();
  }
  return 0;
}

double DataStatistics::GetAverage(const std::string& concept, double default_value) const {
  TokenTaker tt(&token_);
  Averager *averager = averagers_.findInMap(concept);

  if (averager != NULL) {
    return averager->GetAverage();
  }
  ;
  return default_value;
}

double DataStatistics::GetAverageDeviation(const std::string& concept, double default_value) const {
  TokenTaker tt(&token_);
  Averager *averager = averagers_.findInMap(concept);

  if (averager != NULL) {
    return averager->GetDeviation();
  }
  return default_value;
}
}