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


#ifndef _H_AU_LOG_FILE
#define _H_AU_LOG_FILE

#include "au/containers/vector.h"

namespace au 
{

    class Log;
    
    class LogFile
    {        
        
    public:
        
        // Collections of logs from this file
        au::vector<Log> logs;
        
        // Read a file
        static au::Status read( std::string file_name , LogFile** logFile );
        
    };
    
}

#endif
