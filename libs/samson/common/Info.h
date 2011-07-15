
/* ****************************************************************************
 *
 * FILE            Info.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            7/11/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_Samson_Info
#define _H_Samson_Info


#include "au/Environment.h"                 // au::Environment
#include "au/map.h"                         // au::map
#include "au/Info.h"                        // au::Info

#include "samson/common/samson.pb.h"        // network::...

namespace samson {
    
    class Info
    {
        
    public:
        
        // Function to interact with the network message
        static void fill( au::Info*_info , std::string name , network::Info *info );
        static void get( au::Info*_info, const network::Info& info); 

    };


    
}
#endif
