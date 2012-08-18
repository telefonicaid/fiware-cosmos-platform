
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

#ifndef _H_AU_STRING_VECTOR
#define _H_AU_STRING_VECTOR

#include <set>
#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <vector>
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

/*
 
 StringVector is a handy wrapper over std::vector<std::string> to form and work
 a vector of strings
 
*/

namespace au {
  
  class StringVector : public std::vector<std::string>
  {
  public:
    
    // Constructors with different number of arguments
    StringVector();
    StringVector( std::string value );
    StringVector( std::string value , std::string value2 );
    StringVector( std::string value , std::string value2 , std::string value3 );
    StringVector( std::string value , std::string value2 , std::string value3
                 , std::string value4 );
    StringVector( std::string value , std::string value2 , std::string value3
                 , std::string value4 , std::string value5  );
    StringVector( std::string value , std::string value2 , std::string value3
                 , std::string value4 , std::string value5 , std::string value6);
    StringVector( std::string value , std::string value2 , std::string value3
                 , std::string value4 , std::string value5 , std::string value6
                 , std::string value7 );
    StringVector( const std::set<std::string>& values );
    StringVector( const std::vector<std::string>& values );
    StringVector( const StringVector& string_vector );

    const StringVector& operator=( const StringVector& values );
    
    // Parse a line in component using a parituclar separator
    static StringVector ParseFromString( std::string line , char separator = ' ' );
    
    // Append elements contained in other vector
    void Append( const StringVector& values );
    
    // Get string at a particular position
    std::string Get(size_t pos);
    
    // Remove duplicated values in the vector
    void RemoveDuplicated();
    
    // Handy punction to add different types of values
    template <typename T>
    void Push( T v )
    {
      std::ostringstream output;
      output << v;
      push_back( output.str() );
    }
    
    // Debug string
    std::string str();
  };
  
}

#endif
