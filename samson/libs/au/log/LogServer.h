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

#ifndef _H_LOG_SERVER
#define _H_LOG_SERVER

#include <string>
#include <sys/stat.h> // mkdir
#include <fcntl.h>

#include "logMsg/logMsg.h"

#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/containers/map.h"
#include "au/containers/list.h"
#include "au/tables/Table.h"
#include "au/string/split.h"
#include "au/log/Log.h"
#include "au/network/ConsoleService.h"

namespace au {
    
    class LogServerQuery;
    class LogServerChannel;
    
    class LogServer : au::network::ConsoleService
    {
        // Channel to accept connection with binary logs
        LogServerChannel* channel;
        
    public:
        
        // Constructor
        LogServer();
        
        // au::network::ConsoleService
        void runCommand( std::string command , au::Environment* environment ,au::ErrorManager* error );
        void autoComplete( ConsoleAutoComplete* info , au::Environment* environment );
        std::string getPrompt( au::Environment* environment );
        
        
    };
    
}
#endif