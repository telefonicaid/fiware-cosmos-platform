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


#ifndef _H_SAMSON_CONNECTOR_SERVICE
#define _H_SAMSON_CONNECTOR_SERVICE

#include <set>

#include "logMsg/logMsg.h"

#include "au/containers/map.h"
#include "au/containers/vector.h"
#include "au/mutex/Token.h"
#include "au/string.h"
#include "au/console/Console.h"

#include "engine/Buffer.h"

#include "au/network/NetworkListener.h"
#include "au/network/ConsoleService.h"
#include "au/network/RESTService.h"
#include "au/network/NetworkListener.h"

#include "common.h"
#include "StreamConnector.h"
#include "DiskAdaptor.h"
#include "SamsonAdaptor.h"

#include "BufferProcessor.h"

#include "Channel.h"
#include "Adaptor.h"
#include "ConnectorCommand.h"
#include "ConnectorCommand.h"

extern bool interactive;
extern bool run_as_daemon;
extern int sc_console_port;
extern int sc_web_port;


namespace stream_connector {
    
    // Class to accept connection to monitor
    class StreamConnectorService : public au::network::ConsoleService
    {
        StreamConnector * samson_connector;
        
    public:
        
        StreamConnectorService( StreamConnector * _samson_connector );        
        virtual ~StreamConnectorService();
        
        virtual void runCommand( std::string command , au::Environment* environment , au::ErrorManager* error );        
        virtual void autoComplete( au::ConsoleAutoComplete* info , au::Environment* environment );
        virtual std::string getPrompt( au::Environment* environment );
        
    };
    
} // End of namespace stream_connector

#endif