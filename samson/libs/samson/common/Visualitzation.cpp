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

#include <fnmatch.h>
#include <string>

#include "au/log/LogMain.h"

#include "samson/common/Visualitzation.h"

namespace samson {
void Visualization::set_flag(const std::string& name, bool value) {
  if (value) {
    flags_activated_.insert(name);
  } else {
    flags_not_activated_.insert(name);
  }
}

bool Visualization::get_flag(const std::string& name) const {
  // Be robust against omitting "-" in request
  if ((name.length() > 0) && (name[0] != '-')) {
    return get_flag("-" + name);
  }

  if (flags_activated_.find(name) != flags_activated_.end()) {
    return true;
  }

  if (flags_not_activated_.find(name) != flags_not_activated_.end()) {
    return false;
  }

  LOG_SW(("Flag %s not defined for this visualitzation", name.c_str()));
  return false;   // Default value
}

void Visualization::set_pattern(const std::string& pattern) {
  pattern_.set_pattern(pattern);
}

std::string Visualization::pattern() const {
  return pattern_.pattern();
}

bool Visualization::match(const std::string& value) const {
  bool ans = pattern_.match(value);

  return ans;
}

const au::Environment& Visualization::environment() {
  return environment_;
}
}
