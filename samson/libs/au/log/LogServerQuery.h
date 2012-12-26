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
#ifndef _H_LOG_SERVER_QUERY
#define _H_LOG_SERVER_QUERY

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

/*
namespace au {
    
    class LogServer;
    class LogServerQuery;
    class LogServerChannel;
    
    class LogServerQuery : public network::Service
    {
        LogServer * log_server;
        
    public:
        
        LogServerQuery( LogServer * _log_server , int port ) : network::Service( port )
        {
            log_server = _log_server;
        }
        
        virtual ~LogServerQuery()
        {
        }
        
        // Main function for every socket connection received at listening port
        void run( SocketConnection * socket_connection , bool *quit );

        // Get answer to a particular command ( comming from the incomming connection )
        std::string getAnswer( std::string command );
        
    };

}
 */
#endif