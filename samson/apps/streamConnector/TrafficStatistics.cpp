
#include "TrafficStatistics.h"

namespace stream_connector {
TrafficStatistics::TrafficStatistics() : token("TrafficStatistics") {
}

void TrafficStatistics::push_input(size_t size) {
  au::TokenTaker tt(&token);

  input_rate.Push(size);
}

void TrafficStatistics::push_output(size_t size) {
  au::TokenTaker tt(&token);

  output_rate.Push(size);
}

size_t TrafficStatistics::get_input_total() {
  au::TokenTaker tt(&token);

  return input_rate.size();
}

size_t TrafficStatistics::get_output_total() {
  au::TokenTaker tt(&token);

  return output_rate.size();
}

size_t TrafficStatistics::get_input_rate() {
  au::TokenTaker tt(&token);

  return input_rate.rate();
}

size_t TrafficStatistics::get_output_rate() {
  au::TokenTaker tt(&token);

  return output_rate.rate();
}
}