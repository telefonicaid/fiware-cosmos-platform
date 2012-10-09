

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
  bool match( const std::string& value ) const;
  
private:
  regex_t preg;

};

  class SimplePattern {
    
  public:
    
    explicit SimplePattern(const std::string& pattern);
    ~SimplePattern(){};
    
    // Main function to check match
    bool match( const std::string& value ) const;
    
  private:

    std::string pattern_;
    
  };
  
  
  
}

#endif