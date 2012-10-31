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

#include "Pattern.h"  // Own interface
#include <fnmatch.h>

namespace au {
Pattern::Pattern(const std::string& pattern, au::ErrorManager& error) {
  int r = regcomp(&preg, pattern.c_str(), 0);

  if (r != 0) {
    char buffer[1024];
    regerror(errno, &preg, buffer, sizeof( buffer ));
    error.set(buffer);
  }
}

Pattern::~Pattern() {
  regfree(&preg);
}

bool Pattern::match(const std::string& value) const {
  int c = regexec(&preg, value.c_str(), 0, NULL, 0);

  if (c == 0) {
    return true;
  } else {
    return false;
  }
}

SimplePattern::SimplePattern(const std::string& pattern) {
  pattern_ = pattern;
}

bool SimplePattern::match(const std::string& value) const {
  return ( 0 == ::fnmatch(pattern_.c_str(), value.c_str(), 0));
}
}