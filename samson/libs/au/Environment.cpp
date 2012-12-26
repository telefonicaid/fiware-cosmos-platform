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

#include "au/string/StringUtilities.h" // au::split

#include "Environment.h"               // OWn interface

namespace au {
void Environment::Add(const Environment& environment) {
  std::map<std::string, std::string>::const_iterator iter;
  for (iter = environment.items_.begin(); iter != environment.items_.end(); ++iter) {
    Set(iter->first, iter->second);
  }
}

void Environment::ClearEnvironment() {
  items_.clear();
}

std::string Environment::Get(const std::string& name, const std::string& default_value) const {
  std::map<std::string, std::string>::const_iterator iter = items_.find(name);

  if (iter == items_.end()) {
    return default_value;
  } else {
    return iter->second;
  }
}

int Environment::Get(const std::string& name, int defaultValue) const {
  if (!IsSet(name)) {
    return defaultValue;
  }
  return atoi(Get(name, "0").c_str());
}

size_t Environment::Get(const std::string& name, size_t defaultValue) const {
  if (!IsSet(name)) {
    return defaultValue;
  }
  return atoll(Get(name, "0").c_str());
}

double Environment::Get(const std::string& name, double defaultValue) const {
  if (!IsSet(name)) {
    return defaultValue;
  }
  return atof(Get(name, "0").c_str());
}

void Environment::Set(const std::string& name, const std::string& value) {
  std::map<std::string, std::string>::iterator iter = items_.find(name);

  if (iter == items_.end()) {
    items_.insert(std::pair<std::string, std::string>(name, value));
  } else {
    iter->second = value;
  }
}

void Environment::Unset(const std::string& name) {
  std::map<std::string, std::string>::iterator iter = items_.find(name);

  if (iter != items_.end()) {
    items_.erase(iter);
  }
}

bool Environment::IsSet(const std::string& name) const {
  std::map<std::string, std::string>::const_iterator iter = items_.find(name);

  if (iter == items_.end()) {
    return false;
  } else {
    return true;
  }
}

Environment& Environment::operator=(Environment& environment) {
  ClearEnvironment();   // Clear previous enviroment
  std::map<std::string, std::string>::iterator iter;
  for (iter = environment.items_.begin(); iter != environment.items_.end(); ++iter) {
    items_.insert(std::pair<std::string, std::string>(iter->first, iter->second));
  }
  return *this;
}

// Save and recover from string
std::string Environment::SaveToString() const {
  std::ostringstream output;

  std::map<std::string, std::string>::const_iterator e;
  for (e = items_.begin(); e != items_.end(); ) {
    output << e->first << "=" << e->second;
    e++;
    if (e != items_.end()) {
      output << ",";
    }
  }

  return output.str();
}

void Environment::RecoverFromString(const std::string& input) {
  std::vector<std::string> values;
  split(input, ',', values);

  for (size_t v = 0; v < values.size(); v++) {
    std::vector<std::string> property_value;
    split(values[v], '=', property_value);

    if (property_value.size() == 2) {
      Set(property_value[0], property_value[1]);
    }
  }
}

std::string Environment::str() const {
  std::ostringstream output;

  output << *this;
  return output.str();
}

std::ostream& operator<<(std::ostream& o, const Environment& environment) {
  std::map<std::string, std::string>::const_iterator iter;
  o << "{";
  for (iter = environment.items_.begin(); iter != environment.items_.end(); ) {
    o << iter->first << " : " << iter->second;
    iter++;
    if (iter != environment.items_.end()) {
      o << " ";
    }
  }
  o << "}";
  return o;
}
}
