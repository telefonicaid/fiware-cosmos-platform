

#include "Averager.h"  // Own interface

namespace au {
void Averager::Push(double value) {
  mean_ = 0.9 * mean_ + 0.1 * value;
  mean_square_ = 0.9 * mean_square_ + 0.1 * value * value;

  last_values_.push_back(au::str(value));
  while (last_values_.size() > 10) {
    last_values_.pop_front();
  }
}

double Averager::GetAverage() const {
  return mean_;
}

double Averager::GetDeviation() const {
  double value = mean_square_ - mean_ * mean_;

  if (value > 0) {
    return sqrt(value);
  } else {
    return 0;
  }
}

std::string Averager::str(const std::string& unit) const {
  double deviation = GetDeviation();

  if (deviation > 0) {
    return au::str("%s +- %s", au::str(mean_, unit).c_str(), au::str(deviation, unit).c_str());
  } else {
    return au::str(mean_, unit);
  }
}

std::string Averager::str_mean(const std::string& unit) const {
  return au::str(mean_, unit).c_str();
}

std::string Averager::GetLastValues() const {
  return au::str(last_values_);
}
}