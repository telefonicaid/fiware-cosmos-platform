
#include "DataStatistics.h"  // Own interface

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

namespace au {
void RateStatistics::Push(const std::string& concept, double value) {
  TokenTaker tt(&token_);

  rates_.findOrCreate(concept)->Push(value);
}

double RateStatistics::GetRate(const std::string& concept) const {
  TokenTaker tt(&token_);
  Rate *rate = rates_.findInMap(concept);

  if (rate != NULL) {
    return rate->rate();
  }
  return 0;
}

double RateStatistics::GetTotal(const std::string& concept) const {
  TokenTaker tt(&token_);
  Rate *rate = rates_.findInMap(concept);

  if (rate != NULL) {
    return rate->size();
  }
  return 0;
}

void AverageStatistics::Push(const std::string& concept, double value) {
  TokenTaker tt(&token_);

  averagers_.findOrCreate(concept)->Push(value);
}

double AverageStatistics::GetAverage(const std::string& concept, double default_value) const {
  TokenTaker tt(&token_);
  Averager *averager = averagers_.findInMap(concept);

  if (averager != NULL) {
    return averager->GetAverage();
  }
  ;
  return default_value;
}

double AverageStatistics::GetAverageDeviation(const std::string& concept, double default_value) const {
  TokenTaker tt(&token_);
  Averager *averager = averagers_.findInMap(concept);

  if (averager != NULL) {
    return averager->GetDeviation();
  }
  return default_value;
}
}