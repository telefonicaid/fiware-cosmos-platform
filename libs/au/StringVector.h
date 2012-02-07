

#ifndef _H_AU_STRING_VECTOR
#define _H_AU_STRING_VECTOR

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/simple_map.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map
#include "au/string.h"              // au::str(...)
#include "au/ConsoleAutoComplete.h"

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

class StringVector : public std::vector<std::string>
{
public:
    
    StringVector();
    StringVector( std::string value );
    StringVector( std::string value , std::string value2 );
    StringVector( std::string value , std::string value2 , std::string value3 );
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 );
    StringVector( std::set<std::string>& values );

    
    static StringVector parseFromString( std::string line , char separator );
    static StringVector parseFromString( std::string line );
    
    void copyFrom( StringVector& values );
    std::string get(size_t pos);
    
    void unique();
    
    std::string str();
    
};

NAMESPACE_END

#endif
