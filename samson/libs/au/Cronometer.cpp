
#include "au/S.h"
#include "au/string.h"
#include "time.h"

#include "Cronometer.h"     // Own interface

namespace au {
Cronometer::Cronometer() {
  if (gettimeofday(&t_, NULL) != 0)
    LM_X(1, ("gettimeofday failed"));
}

void Cronometer::Reset() {
  if (gettimeofday(&t_, NULL) != 0)
    LM_X(1, ("gettimeofday failed"));
}

double Cronometer::secondsAndReset() {
  double t = seconds();

  Reset();
  return t;
}

double Cronometer::seconds() const {
  struct timeval t;

  if (gettimeofday(&t, NULL) != 0)
    LM_X(1, ("gettimeofday failed")); double diff = ((double)t.tv_sec ) + ((double)t.tv_usec / (double)1000000.0 )
                                                    - ((double)t_.tv_sec ) - ((double)t_.tv_usec / (double)1000000.0);

  return diff;
}

void Cronometer::AddOffset(int offset) {
  t_.tv_sec += offset;
}

bool Cronometer::ResetIfGreaterThan(double time) {
  if (seconds() >= time) {
    Reset();
    return true;
  }
  return false;
}

std::string Cronometer::str() {
  std::ostringstream output;

  output << *this;
  return output.str();
}

std::ostream& operator<<(std::ostream& o, const Cronometer& cronometer) {
  o << str_time_simple(cronometer.seconds());
  return o;
}
}
