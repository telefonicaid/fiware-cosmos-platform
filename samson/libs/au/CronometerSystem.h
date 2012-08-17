

#ifndef AU_CRONOMETER_SYSTEM_H_
#define AU_CRONOMETER_SYSTEM_H_

#include <stdio.h>             /* sprintf */
#include <sys/time.h>	       // struct timeval
#include <string>               // std::string

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/Cronometer.h"

namespace au {
  
  class CronometerSystem
  {
    
  public:
    
    CronometerSystem();
    ~CronometerSystem(){}
    
    // Start stop methods
    void Start();
    void Stop();
    
    // Reset the count to 0
    void Reset();
    
    // Get information
    double running_seconds() const;
    double seconds() const;
    
  private:
    
    Cronometer cronometer_;
    int seconds_;
    bool running_;
    
    friend std::ostream& operator<<( std::ostream& , const CronometerSystem& cronometer_system );
    
  };   
  
}

#endif