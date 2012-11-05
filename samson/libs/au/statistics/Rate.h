
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
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/


#ifndef _H_AU_RATE
#define _H_AU_RATE

#include <cstring>
#include <list>
#include <math.h>
#include <memory>
#include <string>    // std::String
#include <time.h>

#include "au/statistics/Cronometer.h"
#include "au/statistics/Cronometer.h"
#include "au/string/StringUtilities.h"


#include "au/mutex/Token.h"


namespace au {
namespace rate {
class Rate {
public:

  // Constructor
  Rate(int num_seconds_to_average = 10);
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
}
}                     // end of namespace au::rate

#endif  // ifndef _H_AU_RATE
