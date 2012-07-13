#ifndef _H_AU_STRING_UINT64_VECTOR
#define _H_AU_STRING_UINT64_VECTOR

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


namespace au 
{
    
    class Uint64Vector : public std::vector<size_t>
    {
        
    public:
        
        Uint64Vector();
        Uint64Vector(  std::vector<size_t> v );
        
        size_t getNumberOfTimes( size_t t );
        std::set<size_t> getSet();
        void copyFrom(  std::vector<size_t> &v);
        
    };
    
}

#endif