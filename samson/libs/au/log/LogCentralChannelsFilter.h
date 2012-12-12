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

#ifndef _H_LogCentralChannelsFilter
#define _H_LogCentralChannelsFilter

#include <vector>

#include "au/log/LogCommon.h"

namespace au {
/**
 * \brief LogCentralChannelsFilter : Filter to set the log-level for each registered channel
 *
 */

class LogCentralChannelsFilter {
public:

  LogCentralChannelsFilter() {
    for (int i = 0; i < LOG_MAX_CHANNELS; i++) {
      channels_level_[i] = LOG_LEVEL_MESSAGE;  // Messages, Warning and Errors by default
    }
  }

  inline int GetLevel(int channel) const {
    if (channel < 0) {
      return 0;
    }
    if (channel >= LOG_MAX_CHANNELS) {
      return 0;
    }

    return channels_level_[channel];
  }

  inline void SetLevel(int channel, int level) {
    if (channel < 0) {
      return;
    }
    if (channel >= LOG_MAX_CHANNELS) {
      return;
    }
    channels_level_[channel] = level;
  }

  inline void Set(const std::vector<int>& channels, int level) {
    for (size_t i = 0; i  < channels.size(); i++) {
      SetLevel(channels[i], level);
    }
  }

  inline bool IsLogAccepted(int channel, int level) {
    return ( GetLevel(channel) >= level );
  }

  std::string description();

private:

  int channels_level_[LOG_MAX_CHANNELS];
};
}  // end namespace au


#endif  // ifndef _H_LogCentralChannelsFilter
