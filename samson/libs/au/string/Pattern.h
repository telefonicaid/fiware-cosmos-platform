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


#ifndef _H_AU_PATTERN
#define _H_AU_PATTERN

#include "au/ErrorManager.h"
#include <regex.h>
#include <string>

/*
 *
 * class Pattern
 *
 * Wrapper of regexp library to match against a defined pattern
 *
 */

namespace au {
class Pattern {
public:

  Pattern(const std::string& pattern, au::ErrorManager& error);
  ~Pattern();

  // Main function to check match
  bool match(const std::string& value) const;

private:
  regex_t preg;
};

class SimplePattern {
public:

  explicit SimplePattern(const std::string& pattern);
  ~SimplePattern() {
  };

  // Main function to check match
  bool match(const std::string& value) const;

private:

  std::string pattern_;
};
}

#endif // ifndef _H_AU_PATTERN