
#include <fnmatch.h>
#include "Pattern.h" // Own interface

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
  
  bool Pattern::match( const std::string& value )
  {
    int c = regexec(&preg, value.c_str(), 0, NULL, 0);
    if (c == 0)
      return true;
    else
      return false;
  }
  
  
  SimplePattern::SimplePattern(const std::string& pattern) {
    pattern_ = pattern;
  }
  
  
  bool SimplePattern::match( const std::string& value )
  {
    return ( 0 == ::fnmatch( pattern_.c_str() , value.c_str() , 0 ) );
  }
}