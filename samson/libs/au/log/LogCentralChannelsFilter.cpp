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

#include "au/log/LogCentralChannelsFilter.h"  // Own interface


#include "au/log/LogCentral.h"

namespace au {
std::string LogCentralChannelsFilter::description() {
  if (!log_central) {
    return "";
  }
  std::ostringstream output;

  for (int c = 0; c < LOG_MAX_CHANNELS; c++) {
    if (!log_central->log_channels().IsRegistered(c)) {
      continue;
    }

    int level = GetLevel(c);
    if (level > 0) {
      std::string name = log_central->log_channels().channel_name(c);
      output << name << ":" << Log::GetLogLevel(level) << " ";
    }
  }

  return output.str();
}
}
