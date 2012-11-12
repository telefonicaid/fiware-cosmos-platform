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

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/containers/simple_map.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/containers/map.h"                 // au::map
#include "au/string.h"              // au::str(...)
#include "au/console/ConsoleAutoComplete.h"

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

class StringVector : public std::vector<std::string>
{
public:
    
    // Quick Constructor
    StringVector();
    StringVector( std::string value );
    StringVector( std::string value , std::string value2 );
    StringVector( std::string value , std::string value2 , std::string value3 );
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 );
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5  );
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5 , std::string value6);
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5 , std::string value6 , std::string value7 );
    StringVector( std::set<std::string>& values );

    // Parsing lines to get component
    static StringVector parseFromString( std::string line , char separator );
    static StringVector parseFromString( std::string line );

    // Copy from another vector
    void copyFrom( StringVector& values );
    
    // Get position at a particular pos ( "" returned if out of limits )
    std::string get(size_t pos);

    // Remove duplicated values in the vector
    void unique();
    
    // Add different types of values value
    template <typename T>
    void push( T v )
    {
        std::ostringstream output;
        output << v;
        push_back( output.str() );
    }
    
    std::string str();
    std::string str_vector();
};

NAMESPACE_END

#endif
