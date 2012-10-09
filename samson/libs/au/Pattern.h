

#ifndef _H_AU_PATTERN
#define _H_AU_PATTERN

#include <regex.h>
#include <string>
#include "au/ErrorManager.h"

/*
 
 class Pattern
 
 Wrapper of regexp library to match against a defined pattern
 
 */

namespace au{

class Pattern {
  
public:
  
  Pattern(const std::string& pattern, au::ErrorManager& error);
  ~Pattern();

  // Main function to check match
  bool match( const std::string& value );
  
private:
  regex_t preg;

};

}

#endif