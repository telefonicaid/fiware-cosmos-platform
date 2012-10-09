#include "samson/common/Visualitzation.h"

#include <fnmatch.h>

#include <string>

namespace samson {
bool match(const std::string& pattern, const std::string& name) {
  if (pattern == "*") {
    return true;
  }

  return (::fnmatch(pattern.c_str(), name.c_str(), FNM_PATHNAME) == 0);
}

void Visualization::set_flag(const std::string& name, bool value) {
  if (value) {
    flags_activated_.insert(name);
  } else {
    flags_not_activated_.insert(name);
  }
}

bool Visualization::get_flag(const std::string& name) const {
  // Be robust agains omitting "-" in request
  if ((name.length() > 0) && (name[0] != '-')) {
    return get_flag("-" + name);
  }

  if (flags_activated_.find(name) != flags_activated_.end()) {
    return true;
  }

  if (flags_not_activated_.find(name) != flags_not_activated_.end()) {
    return false;
  }

  LM_W(("Flag %s not defined for this visualitzation", name.c_str()));
  return false;   // Default value
}

void Visualization::set_pattern(const std::string& pattern) {
  pattern_ = pattern;
}

const std::string& Visualization::pattern() const {
  return pattern_;
}

bool Visualization::match(const std::string& value) const {
  return (::fnmatch(pattern_.c_str(), value.c_str(), FNM_PATHNAME) == 0);
}

const au::Environment& Visualization::environment() {
  return environment_;
}
}
