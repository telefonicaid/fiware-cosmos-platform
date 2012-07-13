
#include <fnmatch.h>

#include "Visualitzation.h"

namespace samson {

    bool match( std::string pattern , std::string name )
    {
        if( pattern == "*" )
            return true;
        
        return( ::fnmatch( pattern.c_str() , name.c_str() , FNM_PATHNAME ) == 0 );
    }

}