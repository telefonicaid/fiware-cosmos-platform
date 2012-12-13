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


#include "au/log/LogCentralPlugin.h"  // Own interface

namespace au {
// Function to check if a log is accepted based on the channels filter for this plugin
bool LogCentralPlugin::IsLogAccepted(au::SharedPointer<Log> log) const {
  if (!activated_) {
    return false;
  }
  // Check  if the level of acceptance for this channel is high enough
  int channel = log->log_data().channel;
  int level = log->log_data().level;
  return (log_channels_filter_.GetLevel(channel) >= level);
}

// Handy method to know if we have to emit this log here
bool LogCentralPlugin::IsLogAccepted(int channel, int level) {
  return (log_channels_filter_.GetLevel(channel) >= level);
}
}