

#ifndef _H_SAMSON_CONNECTOR_TRAFFIC_STATISTICS
#define _H_SAMSON_CONNECTOR_TRAFFIC_STATISTICS


#include "au/Rate.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

namespace stream_connector {
class TrafficStatistics {
  // Rate statistics
  au::Token token;
  au::rate::Rate input_rate;
  au::rate::Rate output_rate;

public:

  TrafficStatistics();

  void push_input(size_t size);
  void push_output(size_t size);

  size_t get_input_total();
  size_t get_output_total();
  size_t get_input_rate();
  size_t get_output_rate();
};
}

#endif // ifndef _H_SAMSON_CONNECTOR_TRAFFIC_STATISTICS
