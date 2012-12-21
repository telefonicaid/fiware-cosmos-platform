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
#include "au/string/StringUtilities.h"

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

std::string LogCentralChannelsFilter::GetHitDescriptionForChannel(int c) const {
  std::ostringstream output;

  for (int l = 0; l < LOG_MAX_LEVELS; ++l) {
    size_t hits = channels_hits_[c][l];
    if (hits > 0) {
      output << au::str("%s x %s ", au::str(hits).c_str(), Log::GetLogLevel(l).c_str());
    }
  }
  return output.str();
}
}
