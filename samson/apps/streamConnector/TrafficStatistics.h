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


#ifndef _H_STREAM_CONNECTOR_TRAFFIC_STATISTICS
#define _H_STREAM_CONNECTOR_TRAFFIC_STATISTICS


#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/statistics/Rate.h"

namespace stream_connector {
class TrafficStatistics {
  // Rate statistics
  au::Token token;
  au::Rate input_rate;
  au::Rate output_rate;

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

#endif  // ifndef _H_STREAM_CONNECTOR_TRAFFIC_STATISTICS
