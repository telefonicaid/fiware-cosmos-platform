#ifndef _AU_S_H_
#define _AU_S_H_

#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace au {

  /* ------------------------------------------------------------------------

   class au::S
   
   Fancy class to create strings on the fly
   It allows to run something like:
   
   std::string s = au::S() << "int " << 4 << " double " << 0.6;
   
   ------------------------------------------------------------------------ */
  
  class S 
  {
    
  public:
    
    S() {}
    S( const char* s ): oss_(s) {}
    S( const std::string& s ): oss_(s) {}
    
    // Generic Constructor
    template<class T> 
    S( T t )
    {
      oss_ << t;
    }
    
    // Templace to concatenate stuff
    template<class T> 
    S& operator<<(const T& t) 
    {
      oss_ << t;
      return *this;
    }
    
    S& operator<<( std::ostream& (*f) (std::ostream&) )
    {
      oss_ << f;
      return *this;
    }

    // Explicit conversion to string
    std::string str()
    {
      return oss_.str();
    }
    
    // Implicit conversion to string
    operator std::string() const {
      return oss_.str();
    }
  private:
    
    std::ostringstream oss_;
    
  };
  
}

#endif
