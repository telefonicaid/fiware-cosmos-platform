

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

  explicit SimplePattern(const std::string& pattern = "*" );
  ~SimplePattern() {
  };

  // Main function to check match
  bool match(const std::string& value) const;

  void set_pattern( const std::string pattern )
  {
    pattern_ = pattern;
  }
  
  std::string pattern() const
  {
    return pattern_;
  }
  
private:

  std::string pattern_;
};
}

#endif // ifndef _H_AU_PATTERN