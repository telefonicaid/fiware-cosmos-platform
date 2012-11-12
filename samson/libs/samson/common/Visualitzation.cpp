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